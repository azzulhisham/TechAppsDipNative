package com.petronas.dsg.mock;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;

public final class MenuHandler extends AbstractHandler {
	@Override
	public Object execute(final ExecutionEvent event) throws ExecutionException {
		IWorkbenchWindow window = PlatformUI.getWorkbench().getActiveWorkbenchWindow();

		FileDialog fd = new FileDialog(window.getShell(), SWT.OPEN | SWT.MULTI);
		fd.setText("Browser seismic file");
		fd.setFilterExtensions(new String[] { "*.sgy;*.segy", "*.*" });
		fd.open();

		View view = (View) window.getActivePage().findView(View.ID);
		view.openFiles(fd.getFilterPath(), fd.getFileNames());

		return null;
	}
}