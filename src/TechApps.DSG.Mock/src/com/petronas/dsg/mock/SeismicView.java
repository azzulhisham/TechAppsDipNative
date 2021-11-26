package com.petronas.dsg.mock;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.PaletteData;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.part.ViewPart;

import com.petronas.dsg.SeismicData;

public class SeismicView extends ViewPart implements ISelectionListener {
	public static final String ID = "com.petronas.dsg.mock.seismicview";
	private Label _viewer;
	private Image _image;

	public SeismicView() {
		super();
	}

	@Override
	public void createPartControl(Composite parent) {
		_viewer = new Label(parent, SWT.FILL);
		getViewSite().getPage().addSelectionListener(this);
	}

	@Override
	public void setFocus() {
		_viewer.setFocus();
	}

	public void dispose() {
		_image.dispose();
		_viewer.dispose();
	}

	public void selectionChanged(IWorkbenchPart part, ISelection selection) {
		if (selection instanceof IStructuredSelection) {
			Object first = ((IStructuredSelection) selection).getFirstElement();
			if (first instanceof String)
				viewSeismic((String) first, 10);
		}
	}

	public void viewSeismic(String path, int inLine) {
		if (_image != null)
			_image.dispose();
		setPartName("Seismic View of " + path);
		SeismicData seismic = new SeismicData(path);
		float[][] plane = seismic.getByInline(inLine);
		Point p = _viewer.getSize();
		ImageData imageData = getSeismic(plane);//.scaledTo(p.x, p.y);
		Image srcImg = new Image(_viewer.getDisplay(), imageData);
		_image = new Image(_viewer.getDisplay(), p.x, p.y);
		GC gc = new GC(_image);
		gc.setAntialias(SWT.ON); // redraw into a good resolution Image
		gc.setInterpolation(SWT.HIGH); // Interpolation is based in the Graphics
		gc.drawImage(srcImg, 0, 0, imageData.width, imageData.height, 0, 0, p.x, p.y);
		gc.dispose();
		srcImg.dispose();
		_viewer.setImage(_image);
	}

	private static ImageData getSeismic(float[][] traces) {
		float min = Float.MAX_VALUE;
		float max = Float.MIN_VALUE;
		for (int x = 0; x < traces.length; x++) {
			for (int y = 0; y < traces[x].length; y++) {
				if (traces[x][y] < min)
					min = traces[x][y];
				if (traces[x][y] > max)
					max = traces[x][y];
			}
		}

		PaletteData palette = createPalette(256);
		ImageData image = new ImageData(traces.length, traces[0].length, 8, palette);
		int unit = palette.colors.length - 1;
		for (int y = 0; y < image.height; y++) {
			for (int x = 0; x < image.width; x++) {
				double z = traces[x][y] < 0 ? ((traces[x][y] - min) * 0.5) / (0 - min)
						: ((traces[x][y] * 0.5) / max) + 0.5;
				int r = (int) Math.floor(z * unit);
				image.setPixel(x, y, palette.getPixel(palette.colors[r]));
			}
		}
		return image;
	}

	public static PaletteData createPalette(int length, RGB... colors) {
		if (colors.length < 2)
			colors = new RGB[] { new RGB(255, 0, 0), new RGB(255, 255, 255), new RGB(0, 0, 255) };
		int size = length / (colors.length - 1);
		RGB[] c = new RGB[length];
		for (int i = 0, k = 0; i < colors.length - 1; i++) {
			for (int j = 0; j < size; j++, k++) {
				double r = (double) j / (double) (size - 1);
				double nr = 1.0 - r;
				double R = (nr * colors[i].red) + (r * colors[i + 1].red);
				double G = (nr * colors[i].green) + (r * colors[i + 1].green);
				double B = (nr * colors[i].blue) + (r * colors[i + 1].blue);

				c[k] = new RGB((int) R, (int) G, (int) B);
			}
		}

		return new PaletteData(c);
	}
}