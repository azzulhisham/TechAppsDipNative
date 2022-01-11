package com.petronas.dip;

import java.io.IOException;
import java.net.URL;

import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Scale;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.osgi.framework.Bundle;

import com.petronas.dsg.ISeismicData;

public class MainView extends TitleAreaDialog {
	private static final String _name = "Dip Guide";
	private static final String _description = "Dip Guide";
	private static final String _bundle = "com.petronas.dip";
	private static final String _icon = "icons/icon64.png";
	private static final String _arrowIcon = "icons/arrow.png";
	private Image _image = null;
	private Image _arrow = null;
//    private static final Logger _log = LoggerFactory.getLogger(MainView.class);	
	private String _inputName = null;
	private ISeismicData _seismic = null;
	private Dip _attribute = null;

	public MainView(Shell parentShell) {
		super(parentShell);
	}

	public MainView(ISeismicData data) {
		super(Display.getDefault().getActiveShell());
		_seismic = data;

		_attribute = new Dip(_seismic);
		_image = loadImage(_icon);
		_arrow = loadImage(_arrowIcon);
	}

	public void init(final ExecutionEvent event) {
		_seismic.init();
		_inputName = _seismic.getName();
	}

	@Override
	protected void okPressed() {
		if (_inputName == null || _inputName == "")
			return;
		getButton(IDialogConstants.OK_ID).setEnabled(false);
		// _log.debug("calculate");
		_attribute.type = _typeCombo.getSelectionIndex();
		_attribute.windowsX = frequencies[0];
		_attribute.windowsZ = frequencies[1];
		_attribute.dX = frequencies[2];
		_attribute.dZ = frequencies[3];
		_attribute.psizeCut = frequencies[4] / divider[4];

		_time = System.nanoTime();
		_job = new Job(_name) {
			@Override
			protected IStatus run(IProgressMonitor monitor) {
				try {
					_attribute.run(monitor);
					_seismic.save();
				} catch (Exception e) {
					// _log.error(e.getMessage());
					System.out.println("ok button press ex");
					e.printStackTrace();
				} finally {
					monitor.done();
				}

				syncWithUI(); // sync with UI
				return Status.OK_STATUS;
			}
		};
		_job.setUser(true);
		_job.schedule();

	}

	@Override
	protected void cancelPressed() {
		if (_job != null)
			_job.cancel();
		close();
	}

	long _time = 0;
	Job _job;

	private void syncWithUI() {
		Display.getDefault().asyncExec(new Runnable() {
			public void run() {				
				double elapsedTimeInSecond = duration();
				MessageDialog.openInformation(Display.getCurrent().getActiveShell(), "message",
						"completed in: " + elapsedTimeInSecond + "ns per million");
				Button ok = getButton(IDialogConstants.OK_ID);
				if (ok != null)
					ok.setEnabled(true);
			}
		});
	}

	private double duration() {
		double elapsedTimeInSecond = (double) (System.nanoTime() - _time) / 1_000_000_000;
		// _log.info("completed in: " + elapsedTimeInSecond + "ns per million");
		return elapsedTimeInSecond;
	}

	private void showInput(final Composite parent) {
		Composite container = new Composite(parent, SWT.BORDER);
		container.setLayoutData(new GridData(SWT.FILL, SWT.TOP, true, false));
		container.setLayout(new GridLayout(8, true));

		Label label = new Label(container, SWT.NONE);
		label.setText("Input");
		Button getInput = new Button(container, SWT.NONE);
		getInput.setImage(_arrow);
		final GridData btnGridData = new GridData(SWT.RIGHT, SWT.FILL, true, false);
		getInput.setLayoutData(btnGridData);
		Text value = new Text(container, SWT.BORDER);
		final GridData textGridData = new GridData(SWT.FILL, SWT.FILL, true, false);
		textGridData.horizontalSpan = 5;
		value.setLayoutData(textGridData);
		value.setEnabled(false);
		value.setText(_inputName == null ? "" : _inputName);
		Button help = new Button(container, SWT.NONE);
		help.setToolTipText("seismic input");
		help.setText("  ?  ");
		help.addPaintListener(helpPaint);

		getInput.addSelectionListener(new SelectionListener() {
			@Override
			public void widgetDefaultSelected(SelectionEvent arg0) {
			}

			@Override
			public void widgetSelected(SelectionEvent arg0) {
				_seismic.init();
				_inputName = _seismic.getName();
				// value.setText(_inputName == null ? "" : _inputName);
				Display.getDefault().asyncExec(() -> value.setText(_inputName == null ? "" : _inputName));
			}
		});
	}

	private int[] frequencies = new int[] { 5, 3, 1, 1, 1 };
	private static final int[] min = new int[] { 3, 3, 1, 1, 1 };
	private static final int[] max = new int[] { 25, 25, 5, 5, 10 };
	private static final float[] divider = new float[] { 1, 1, 1, 1, 10000 };
	private static final String[] formatter = new String[] { "%-6s", "%-6s", "%-6s", "%-6s", "%.4f" };

	private static final int nf = 5;
	private static final Text[] values = new Text[nf];
	private static final Scale[] sliders = new Scale[nf];
	private static final PaintListener helpPaint = new PaintListener() {
		@Override
		public void paintControl(PaintEvent e) {
			e.gc.setBackground(new Color(Display.getCurrent(), 104, 179, 253));
			e.gc.fillRectangle(e.x, e.y, e.width, e.height);
			e.gc.setForeground(new Color(Display.getCurrent(), 255, 255, 255));
			e.gc.drawText("?", e.x + (e.width / 2) - 4, e.y + 4);
		}
	};

	private static final String[] helpTexts = { "Window X", "Windows Z", "DX", "DZ", "PSize Cut" };
	private static final String[] itemTypes = { "PWD X and Y", "PWDx", "PWDy" };
	private Combo _typeCombo;

	private void createParameter(final Composite parent) {
		// set the grid layout with 8 columns
		Composite container = new Composite(parent, SWT.BORDER);
		container.setLayoutData(new GridData(SWT.FILL, SWT.TOP, true, false));
		container.setLayout(new GridLayout(8, true));

		// place the label for title 'Parameter', fill all the columns
		Label parameter = new Label(container, SWT.NONE);
		parameter.setText("Parameter");
		final GridData parameterGridData = new GridData(SWT.FILL, SWT.FILL, true, false);
		parameterGridData.horizontalSpan = 8;
		parameter.setLayoutData(parameterGridData);

		// make an empty line
		Label emptyLine = new Label(container, SWT.NONE);
		final GridData emptyLineGridData = new GridData(SWT.FILL, SWT.FILL, true, false);
		emptyLineGridData.horizontalSpan = 8;
		emptyLine.setLayoutData(emptyLineGridData);

		// input for Type parameter using combo box.
		Label parameterType = new Label(container, SWT.NONE);
		parameterType.setText("Type");
		final GridData parameterGridType = new GridData(SWT.FILL, SWT.FILL, true, false);
		parameterGridType.horizontalSpan = 2;
		parameterType.setLayoutData(parameterGridType);
		_typeCombo = new Combo(container, SWT.READ_ONLY);
		final GridData comboGridData = new GridData(SWT.FILL, SWT.FILL, true, false);
		comboGridData.horizontalSpan = 4;
		_typeCombo.setLayoutData(comboGridData);
		_typeCombo.setItems(itemTypes);
		_typeCombo.select(0);

		// make a empty space with 2 columns width
		Label emptySpace = new Label(container, SWT.NONE);
		final GridData emptyGridData = new GridData(SWT.FILL, SWT.FILL, true, false);
		emptyGridData.horizontalSpan = 2;
		emptySpace.setLayoutData(emptyGridData);

		final GridData labelGridData = new GridData(SWT.FILL, SWT.CENTER, true, false);
		labelGridData.horizontalSpan = 2;
		final GridData sliderGridData = new GridData(SWT.FILL, SWT.TOP, true, false);
		sliderGridData.horizontalSpan = 4;
		final GridData textGridData = new GridData(SWT.FILL, SWT.CENTER, true, false);

		for (int i = 0; i < frequencies.length; i++) {
			final int x = i;
			Label label = new Label(container, SWT.NONE);
			label.setText(helpTexts[i]);
			label.setLayoutData(labelGridData);
			sliders[i] = new Scale(container, SWT.NONE);
			sliders[i].setMinimum(min[i]);
			sliders[i].setMaximum(max[i]);
			sliders[i].setLayoutData(sliderGridData);
			sliders[i].setSelection(frequencies[x]);
			sliders[i].addSelectionListener(new SelectionAdapter() {
				@Override
				public void widgetSelected(SelectionEvent e) {
					frequencies[x] = sliders[x].getSelection();
					values[x].setData(true);
					values[x].setText(String.format(formatter[x], frequencies[x] / divider[x]));
				}
			});

			values[i] = new Text(container, SWT.BORDER);
			values[i].setLayoutData(textGridData);
			values[i].setText(String.format(formatter[i], frequencies[i] / divider[i]));
			values[i].pack();
			values[i].addModifyListener(new ModifyListener() {
				@Override
				public void modifyText(ModifyEvent e) {
					Object data = values[x].getData();
					if (data != null) {
						values[x].setData(null);
						return;
					}
					frequencies[x] = Integer.parseInt(values[x].getText().trim());
					sliders[x].setSelection(frequencies[x]);
				}
			});
			Button help = new Button(container, SWT.NONE);
			help.setToolTipText(helpTexts[i]);
			help.setText("     ");
			help.addPaintListener(helpPaint);
		}		
	}

	private static Image loadImage(String path) {
		Bundle b = Platform.getBundle(_bundle);
		URL file = FileLocator.find(b, new Path(path), null);
		Image image = null;
		try {
			URL f = FileLocator.toFileURL(file);
			image = new Image(Display.getDefault(), f.getPath());
		} catch (IOException e) {
			// _log.error("cannot load image: " + e.getMessage());
		}
		return image;
	}

	@Override
	protected Control createDialogArea(final Composite parent) {
		parent.getShell().setText(_name);
		Composite area = (Composite) super.createDialogArea(parent);
		Composite container = new Composite(area, SWT.NONE);
		container.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		GridLayout layout = new GridLayout(1, false);
		layout.verticalSpacing = 10;
		layout.marginTop = 10;
		container.setLayout(layout);
		showInput(container);
		createParameter(container);
		return area;
	}

	@Override
	public void create() {
		super.create();
		setTitle(_name);
		setMessage(_description);
		if (_image != null)
			setTitleImage(_image);
	}

	@Override
	protected Point getInitialSize() {
		return new Point(1024, 600); // configurable
	}

	@Override
	public boolean close() {
		if (_image != null)
			_image.dispose();
		if (_arrow != null)
			_arrow.dispose();
		return super.close();
	}

	/**
	 * Modal windows allow click behind, Allow Resize
	 */
	@Override
	public void setShellStyle(int newShellStyle) {
		super.setShellStyle(SWT.CLOSE | SWT.MODELESS | SWT.BORDER | SWT.TITLE | SWT.RESIZE);
		setBlockOnOpen(false);
	}
}
