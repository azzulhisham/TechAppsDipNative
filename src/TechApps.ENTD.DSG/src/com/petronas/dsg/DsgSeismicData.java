package com.petronas.dsg;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.runtime.IAdapterManager;
import org.eclipse.core.runtime.Platform;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.PlatformUI;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.lgc.dsaf.app.core.session.DspSession;
import com.lgc.dsaf.app.core.session.DspSessionService;
import com.lgc.dsaf.app.services.project.DspApplicationProjectService;
import com.lgc.dsaf.data.access.config.DaDataProject;
import com.lgc.dsaf.data.app.DspDataException;
import com.lgc.dsaf.data.app.DspDataObject;
import com.lgc.dsaf.data.app.DspDataObjectNotFoundException;
import com.lgc.dsaf.data.app.DspDataObjectStaleUpdaterException;
import com.lgc.dsaf.data.app.DspDataServicesProvider;
import com.lgc.dsaf.data.app.DspDataTypeNotFoundException;
import com.lgc.dsaf.data.app.DspObjectId;
import com.lgc.dsaf.data.app.support.DspFakeDataUtils;
import com.lgc.dsaf.type.seismic3ddataset.data.DspSeismic3dDataSet;
import com.lgc.dsaf.type.seismic3ddataset.data.Seismic3dDataSetBuilder;
import com.lgc.dsaf.type.seismic3ddataset.data.Seismic3dDataSetDataTypeMgr;
import com.lgc.infra.fdic.BufferGrid;
import com.lgc.infra.fdic.FloatSubGrid;
import com.lgc.infra.geometry.Index3;
import com.lgc.infra.geometry.IndexRange;
import com.lgc.infra.geometry.Indexes;

public class DsgSeismicData implements ISeismicData {
    private static DaDataProject _project;
	private Seismic3dDataSetDataTypeMgr mgr = null;	
	private static final Logger _log = LoggerFactory.getLogger(DsgSeismicData.class);	
	private Seismic3dDataSetBuilder[] targetUpdater;
	private DspSeismic3dDataSet[] targetObj;
	private DspSeismic3dDataSet sourceObj;
	private float nullValue = 0;
	boolean hasNullValue = false;
	
	
	private int _numberOfTrace;
	public int getNumberOfTrace() {
		return _numberOfTrace;
	}
	
	private int _numberOfSample;
	public int getNumberOfSample() {
		return _numberOfSample;
	}
	
	private int _numberOfInline;
	public int getNumberOfInLine() {
		return _numberOfInline;
	}
	
	private int _numberOfCrossLine;
	public int getNumberOfCrossLine() {
		return _numberOfCrossLine;
	}
	
	private float _sampleRate;
	public float getSampleRate() {
		return _sampleRate;
	}

	public DsgSeismicData() {
		_project = getActiveProject();
		try {
			mgr = (Seismic3dDataSetDataTypeMgr) DspDataServicesProvider.getDspDataServices()
					.getDataTypeMgr(DspSeismic3dDataSet.class);
		} catch (DspDataTypeNotFoundException e) {
			_log.error("exception :" + e.getMessage());
		} catch (DspDataException e) {
			_log.error("exception :" + e.getMessage());
		}		
	}
	
	public void init() {
		IStructuredSelection ss = (IStructuredSelection) PlatformUI.getWorkbench()
				.getActiveWorkbenchWindow().getSelectionService().getSelection();		
		if (ss == null)
			return;		

		IAdapterManager adapterManager = Platform.getAdapterManager();		
		for (Iterator<?> iter = ss.iterator(); iter.hasNext();) {
	    	DspDataObject dspDataObject = adapterManager.getAdapter(iter.next(), DspDataObject.class);
	    	if (dspDataObject instanceof DspSeismic3dDataSet)
	    		sourceObj = (DspSeismic3dDataSet) dspDataObject;
	    }
		if (sourceObj == null)
			return;
		DspSeismic3dDataSet.Header header = sourceObj.getHeader();
		nullValue = header.getDataNull();
		hasNullValue = nullValue != 0;
        Index3 length = header.getNonNullRange().getLength();
        _sampleRate = (float) header.getGridDef().getDeltaZ() / 1000;
        _numberOfSample = length.getK();        
        _numberOfInline = length.getI();
        _numberOfCrossLine = length.getJ();
        _numberOfTrace = length.getI() * length.getJ();
	}	
	
	public String getName() {		
		return sourceObj == null ? null : sourceObj.getCore().getName();
	}
	
	public void setOutputName(String... name) {
		int rn = DspFakeDataUtils.getUniqueId();
		targetObj = new  DspSeismic3dDataSet[name.length];
		targetUpdater = new Seismic3dDataSetBuilder[targetObj.length];
		try {
			for (int i = 0; i < targetObj.length; i++) {
				targetObj[i] = saveAs(sourceObj, String.format("_%s_%s", rn, name[i]));
				targetUpdater[i] = mgr.getObjectUpdater(targetObj[i], this);
			}
		} catch (DspDataException e) {
			_log.error("exception :" + e.getMessage());
		}
	}
	
	public void save(SeismicRange range, int index) {
		IndexRange<Index3> subRange = ranges.get(range.id);
		targetUpdater[index].getData().getDataValues().getFloatSubGrid(subRange).setFloatArray(range.getItem());
	}
	
	List<IndexRange<Index3>> ranges;
	public SeismicRange[] sliceByInline() {
		DspSeismic3dDataSet.Header header = sourceObj.getHeader();
		IndexRange<Index3> nonNullRange = header.getNonNullRange();
		BufferGrid bufferGrid = sourceObj.getData().getDataValues();
		float nullValue = header.getDataNull();
		boolean hasNullValue = nullValue != 0;
		Index3 offset = nonNullRange.getFirst();
		Index3 inc = nonNullRange.getInc();
		Index3 length = nonNullRange.getLength();
		ranges = new ArrayList<IndexRange<Index3>>();
		SeismicRange[] items = new SeismicRange[length.getI()];
		for (int i = offset.getI(), j = 0; j < items.length; i += inc.getI(), j++) {
			Index3 suboffset = Indexes.index(i, offset.getJ(), offset.getK());
			Index3 subLength = Indexes.index(1, length.getJ(), length.getK());
			IndexRange<Index3> subRange = new IndexRange<Index3>(suboffset, inc, subLength);
			ranges.add(subRange);
			SeismicRange item = new SeismicRange();
			item.fn = () -> {				
				FloatSubGrid floatSubGrid = bufferGrid.getFloatSubGrid(subRange);
				float[] floatArray = floatSubGrid.getFloatArray();				
				if (!hasNullValue)
					return floatArray;
				for (int k = 0; k < floatArray.length; k++) {			
					if (floatArray[k] == nullValue)
						floatArray[k] = 0;
				}
				return floatArray;
			};
			item.id = j;
			item.traceSize = subRange.getLength().getJ();
			items[j] =  item;			
		}
		return items;
	}	
	
	public SeismicRange[] subCubeByTrace(int size) {		
		IndexRange<Index3> nonNullRange = sourceObj.getHeader().getNonNullRange();
		BufferGrid bufferGrid = sourceObj.getData().getDataValues();		
		Index3 offset = nonNullRange.getFirst();
		Index3 inc = nonNullRange.getInc();
		Index3 length = nonNullRange.getLength();
		ranges = new ArrayList<IndexRange<Index3>>();
		SeismicRange[] items = new SeismicRange[_numberOfTrace];		
		int padding = size / 2;
		int cubeSize = size * size;
		for (int i = offset.getI(), ii = 0, k = 0; ii < _numberOfInline; i += inc.getI(), ii++) {
			ranges.add(new IndexRange<Index3>(Indexes.index(i, offset.getJ(), offset.getK()), inc, Indexes.index(1, length.getJ(), length.getK())));
			for (int jj = 0; jj < _numberOfCrossLine; jj++, k++) {
				List<Index3> cubeIndex = new ArrayList<Index3>();
				List<IndexRange<Index3>> cubeRanges = new ArrayList<IndexRange<Index3>>();
				int m = ii - padding;
				int n = jj - padding;				
				if (m < 0 || m > _numberOfInline - size - 1 || n < 0 || n > _numberOfCrossLine - size - 1) {
					for (int h = 0; h < size; h++, m++) {
						if (m < 0 || m > _numberOfInline - 1)
							continue;
						int w = padding - jj;
						int nl = n < 0 ? n + size : (n < _numberOfCrossLine - size - 1 ? size : _numberOfCrossLine - n - 1);
						Index3 suboffset = Indexes.index(offset.getI() + m,  offset.getJ() + (n < 0 ? 0 : n), offset.getK());
						Index3 subLength = Indexes.index(1, nl, length.getK()); 
						IndexRange<Index3> subRange = new IndexRange<Index3>(suboffset, inc, subLength);						
						Index3 index = Indexes.index(h, w < 0 ? 0 : w, 0);
						cubeRanges.add(subRange);
						cubeIndex.add(index);						
						
					}
				} else {
					Index3 suboffset = Indexes.index(offset.getI() + m, offset.getI() + n, offset.getK());
					Index3 subLength = Indexes.index(size, size, length.getK());
					IndexRange<Index3> subRange = new IndexRange<Index3>(suboffset, inc, subLength);
					cubeRanges.add(subRange);
				}	
				
				SeismicRange item = new SeismicRange();
				item.id = k;
				item.traceSize = 1;
				if (cubeRanges.size() == 1) {
					item.fn = () -> {
						float[] cube = bufferGrid.getFloatSubGrid(cubeRanges.get(0)).getFloatArray();						
						return cube;
					};		
				} else {
					item.fn = () -> {
						float[] cube = new float[_numberOfSample * cubeSize];
						for(int r = 0; r < cubeRanges.size(); r++) {
							float[] input = bufferGrid.getFloatSubGrid(cubeRanges.get(r)).getFloatArray();
							Index3 index = cubeIndex.get(r);
							System.arraycopy(input, 0, cube, ((index.getI() * size) + index.getJ()) * _numberOfSample , input.length);
						}
						return cube;
					};		
				}
				items[k] = item;
			}
		}
		return items;
	}
	
	/**
	 * This method demonstrate to create a new seismic dataset from given
	 * source. Create only Core and Header data.. object.
	 * 
	 * @param srcObj
	 *            (Read from OW)
	 * @return (Returns the created Object)
	 * @throws DspDataException
	 */
	private DspSeismic3dDataSet saveAs(DspSeismic3dDataSet srcObj, String suffix) throws DspDataException {
		Seismic3dDataSetBuilder updater = mgr.getObjectCreator(_project, this);
		DspSeismic3dDataSet savedObj = null;		
		// Core Data
		updater.getCore().setAttribute(srcObj.getCore().getAttribute());
		//update name
		updater.getCore().setName(srcObj.getCore().getName() + suffix);
		updater.getCore().setVersion(srcObj.getCore().getVersion());
		updater.getCore().setZDomainType(srcObj.getCore().getZDomainType());
		updater.getCore().setBinaryDataFormat(srcObj.getCore().getBinaryDataFormat());
		updater.getCore().setDataFileFormat(srcObj.getCore().getDataFileFormat());
		updater.getCore().setSurveyFullMoniker(srcObj.getCore().getSurveyFullMoniker());

		// Header Data
		updater.getHeader().setRemark(srcObj.getHeader().getRemark());
		updater.getHeader().setGridDef(srcObj.getHeader().getGridDef());
		updater.getHeader().setIndexAnnotator(srcObj.getHeader().getIndexAnnotator());
		updater.getHeader().setPolarity(srcObj.getHeader().getPolarity());
		updater.getHeader().setFidelity(srcObj.getHeader().getFidelity());
		updater.getHeader().setDatum(srcObj.getHeader().getDatum());
		updater.getHeader().setDataNull(srcObj.getHeader().getDataNull());
		updater.getHeader().setDataMin(srcObj.getHeader().getDataMin());
		updater.getHeader().setDataMax(srcObj.getHeader().getDataMax());
		updater.getHeader().setNonNullRange(srcObj.getHeader().getNonNullRange());

		try {
			// Save to OW
			savedObj = mgr.createObject(updater);
		} catch (Exception e) {			
			_log.error("Exception occured in saveAs() method. " + e.getMessage());
		}

		return savedObj;
	}
	
	private DaDataProject getActiveProject() {		
		return PlatformUI.getWorkbench().getService(DspApplicationProjectService.class).getActiveProject();
	}
	
	public void save() {
		try {
			DspSessionService dspSessionService = PlatformUI.getWorkbench().getService(DspSessionService.class);        
			DspSession currentSession = dspSessionService.getCurrentSession();
			for (int i = 0; i < targetUpdater.length; i++) {
				mgr.updateObject(targetUpdater[i]);
				_log.info("Writing trace by trace volume done..!");
			}
			mgr.commitChanges(_project);
			for (int i = 0; i < targetObj.length; i++) {
				@SuppressWarnings("unchecked")
				DspObjectId<DspSeismic3dDataSet> dspObjectId = (DspObjectId<DspSeismic3dDataSet>) targetObj[i].getId();
				currentSession.addItem(dspObjectId); // Add object to the current session
			}
		} catch (DspDataObjectNotFoundException e) {
			_log.error("Exception " + e.getMessage());
		} catch (DspDataObjectStaleUpdaterException e) {
			_log.error("Exception " + e.getMessage());
		} catch (DspDataException e) {
			_log.error("Exception " + e.getMessage());
		}			
		
	}
}
