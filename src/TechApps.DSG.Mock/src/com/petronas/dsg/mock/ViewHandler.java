package com.petronas.dsg.mock;

import java.util.Iterator;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;

public final class ViewHandler extends AbstractHandler {
	@Override
	public Object execute(final ExecutionEvent event) throws ExecutionException {
		IWorkbenchWindow window  = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
		IStructuredSelection ss = (IStructuredSelection) window.getSelectionService().getSelection();
		if (ss == null)
			return null;
		String path = null;
		for (Iterator<?> iter = ss.iterator(); iter.hasNext();) {
			Object obj = iter.next();
			if (obj instanceof String)
				path = (String) obj;
		}
		if (path == null) 
			return null;
		
		SeismicView view = (SeismicView) window.getActivePage().findView(SeismicView.ID);
		view.viewSeismic(path, 20);
		
		return null;
	}
}