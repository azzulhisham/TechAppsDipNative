package com.petronas.dsg.mock;

import java.io.File;

import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ColumnLabelProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.IViewSite;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.ViewPart;

public class View extends ViewPart {
	public static final String ID = "com.petronas.dsg.mock.view";

	// @Inject IWorkbench workbench;

	private class TreeContentProvider implements ITreeContentProvider {
		@Override
		public boolean hasChildren(Object element) {
			return false;
		}

		@Override
		public Object getParent(Object element) {
			return null;
		}

		@Override
		public Object[] getElements(Object inputElement) {
			return ArrayContentProvider.getInstance().getElements(inputElement);
		}

		@Override
		public Object[] getChildren(Object parentElement) {
			return null;
		}
	}

	private class StringLabelProvider extends ColumnLabelProvider {
		@Override
		public Image getImage(Object obj) {
			IWorkbench w = PlatformUI.getWorkbench();
			return w == null ? null : w.getSharedImages().getImage(ISharedImages.IMG_OBJ_FILE);
		}
	}

	private TreeViewer viewer;

	@Override
	public void createPartControl(Composite parent) {
		viewer = new TreeViewer(parent);
		viewer.setContentProvider(new TreeContentProvider());
		viewer.setLabelProvider(new StringLabelProvider());

		MenuManager menuManager = new MenuManager(); // Create a menu manager and create context menu
		Menu menu = menuManager.createContextMenu(viewer.getTree());
		viewer.getTree().setMenu(menu); // set the menu on the SWT widget
		getSite().registerContextMenu(menuManager, viewer); // register the menu with the framework
		getSite().setSelectionProvider(viewer);// make the selection available to other views

		restoreState();
	}

	public void openFiles(String path, String[] files) {
		for (int i = 0; i < files.length; i++)
			files[i] = path + File.separator + files[i];
		viewer.setInput(files); // Provide the input to the ContentProvider
	}

	@Override
	public void setFocus() {
		viewer.getControl().setFocus();
	}

	@Override
	public void saveState(final IMemento memento) {
		String[] files = (String[]) viewer.getInput();
		if (files == null || files.length == 0)
			return;
		IMemento seismics = memento.createChild("seismics");
		for (int i = 0; i < files.length; i++)
			seismics.createChild("seismic", files[i]);
	}

	private IMemento _memento;

	@Override
	public void init(final IViewSite site, final IMemento memento) throws PartInitException {
		init(site);
		_memento = memento;
	}

	private void restoreState() {
		IMemento m;
		if (_memento == null || (m = _memento.getChild("seismics")) == null || m.getChildren("seismic").length == 0)
			return;
		IMemento seismics[] = m.getChildren("seismic");
		String[] files = new String[seismics.length];
		for (int i = 0; i < files.length; i++)
			files[i] = seismics[i].getID();
		viewer.setInput(files);
	}

}