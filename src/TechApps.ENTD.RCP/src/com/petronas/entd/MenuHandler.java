package com.petronas.entd;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;

import com.petronas.dsg.SeismicData;

public final class MenuHandler extends AbstractHandler {
	@Override
	public Object execute(final ExecutionEvent event) throws ExecutionException {
    	MainView diag = new MainView(new SeismicData());
    	diag.init(event);
    	diag.open();
    	return null;
    }
}