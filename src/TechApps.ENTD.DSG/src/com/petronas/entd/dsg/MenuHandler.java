package com.petronas.entd.dsg;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;

import com.petronas.dsg.DsgSeismicData;
import com.petronas.entd.MainView;

public final class MenuHandler extends AbstractHandler {
	@Override
	public Object execute(final ExecutionEvent event) throws ExecutionException {
    	MainView diag = new MainView(new DsgSeismicData());
    	diag.init(event);
    	diag.open();
    	return null;
    }
}