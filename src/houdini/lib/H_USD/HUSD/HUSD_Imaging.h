/*
 * PROPRIETARY INFORMATION.  This software is proprietary to
 * Side Effects Software Inc., and is not to be reproduced,
 * transmitted, or disclosed in any way without written permission.
 *
 * Produced by:
 *	Side Effects Software Inc.
 *	123 Front Street West, Suite 1401
 *	Toronto, Ontario
 *      Canada   M5J 2M2
 *	416-504-9876
 *
 */

#ifndef __HUSD_Imaging_h__
#define __HUSD_Imaging_h__

#include "HUSD_API.h"
#include "HUSD_DataHandle.h"
#include "HUSD_RendererInfo.h"
#include "HUSD_Scene.h"
#include <UT/UT_NonCopyable.h>
#include <UT/UT_BoundingBox.h>
#include <UT/UT_StringArray.h>
#include <UT/UT_UniquePtr.h>
#include <UT/UT_Matrix3.h>
#include <UT/UT_Matrix4.h>
#include <UT/UT_Options.h>
#include <UT/UT_Rect.h>

class HUSD_Scene;
class HUSD_Compositor;

class HUSD_API HUSD_Imaging : public UT_NonCopyable
{
public:
			 HUSD_Imaging();
			~HUSD_Imaging();

    // The scene is not owned by this class.
    void setScene(HUSD_Scene *scene_ref);

    // only the USD modes that map to ours
    enum DrawMode
    {
	DRAW_WIRE,
	DRAW_SHADED_NO_LIGHTING,
	DRAW_SHADED_FLAT,
	DRAW_SHADED_SMOOTH,
	DRAW_WIRE_SHADED_SMOOTH
    };

    void		 showPurposeRender(bool enable);
    void		 showPurposeProxy(bool enable);
    void		 showPurposeGuide(bool enable);

    void		 setDrawMode(DrawMode mode);
    void		 setDrawComplexity(float complexity);
    void		 setBackfaceCull(bool cull);
    void		 setStage(const HUSD_DataHandle &data_handle,
				const HUSD_ConstOverridesPtr &overrides);
    void		 setSelection(const UT_StringArray &paths);
    bool		 setFrame(fpreal frame);
    bool		 setHeadlight(bool doheadlight);
    void		 setLighting(bool enable);

    enum BufferSet
    {
        BUFFER_COLOR_DEPTH,
        BUFFER_COLOR,
        BUFFER_NONE,
    };
    BufferSet            hasAOVBuffers() const;
    
    bool                 canBackgroundRender(const UT_StringRef &name) const;

    // Fire off a render and return immediately.
    // Only call if canBackgroundRender() returns true.
    bool                 launchBackgroundRender(const UT_Matrix4D &view_matrix,
                                                const UT_Matrix4D &proj_matrix,
                                                const UT_DimRect &viewport_rect,
                                                const UT_StringRef &renderer,
                                                const UT_Options *render_opts,
                                                bool update_deferred = false);
    // Check if the BG render is finished, optionally waiting for it.
    bool                 checkRender(bool wait, bool do_render);

    void                 updateComposite(bool free_buffers_if_missing);


    // Fire off a render and block until done. It may return false if the
    // render delegate fails to initialize, it which case another delegate
    // should be chosen.
    bool		 render(const UT_Matrix4D &view_matrix,
				const UT_Matrix4D &proj_matrix,
				const UT_DimRect &viewport_rect,
				const UT_StringRef &renderer,
				const UT_Options *render_opts,
                                bool update_deferred);

    
    void		 setAOVCompositor(HUSD_Compositor *comp)
			 { myCompositor = comp; }

    HUSD_Scene		&scene()
			 { return *myScene; }
    bool		 isConverged() const
			 { return !running() && myConverged; }
    void		 terminateRender(bool hard_halt = true);

    bool		 getBoundingBox(UT_BoundingBox &bbox,
				const UT_Matrix3R *rot) const;

    const UT_StringHolder &rendererName() const
			 { return myRendererName; }

    enum RunningStatus {
	RUNNING_UPDATE_NOT_STARTED = 0,
	RUNNING_UPDATE_IN_BACKGROUND,
	RUNNING_UPDATE_COMPLETE,
        RUNNING_UPDATE_FATAL
    };
    bool		 running() const;
    bool                 isComplete() const;

    // Pause render. Return true if it is paused.
    bool                 pauseRender();
    // Resume a paused render.
    void                 resumeRender();
    bool                 canPause() const;
    bool                 isPaused() const;

    static bool		 getAvailableRenderers(HUSD_RendererInfoMap &info_map);

    const UT_StringArray &rendererPlanes() const { return myPlaneList; }
    void                 setOutputPlane(const UT_StringRef &name)
                         { myOutputPlane = name; }
    const UT_StringRef  &outputPlane() const { return myOutputPlane; }

    void                 getRenderStats(UT_Options &stats);

private:
    class husd_ImagingPrivate;

    void		 updateLightsAndCameras();
    void		 updateDeferredPrims();
    bool		 setupRenderer(const UT_StringRef &renderer_name,
                                const UT_Options *render_opts);

    template <typename T>
    void                 updateSettingIfRequired(const char *key,
                                const T &value);
    void                 updateSettingsIfRequired();
    RunningStatus	 updateRenderData(const UT_Matrix4D &view_matrix,
                                const UT_Matrix4D &proj_matrix,
                                const UT_DimRect &viewport_rect,
                                bool update_deferred);
    void		 finishRender(bool do_render);

    UT_UniquePtr<husd_ImagingPrivate>	 myPrivate;
    fpreal				 myFrame;
    HUSD_DataHandle			 myDataHandle;
    HUSD_ConstOverridesPtr		 myOverrides;
    UT_StringArray			 mySelection;
    unsigned				 myWantsHeadlight : 1,
					 myHasHeadlight : 1,
					 myDoLighting : 1,
					 myHasLightCamPrims : 1,
					 myHasGeomPrims : 1,
					 mySelectionNeedsUpdate : 1,
					 myConverged : 1,
                                         mySettingsChanged : 1,
                                         myIsPaused : 1;
    HUSD_Scene				*myScene;
    UT_StringHolder			 myRendererName;
    HUSD_Compositor			*myCompositor;
    UT_Options				 myCurrentOptions;
    SYS_AtomicInt32			 myRunningInBackground;
    UT_UniquePtr<HUSD_AutoReadLock>	 myReadLock;
    UT_StringArray                       myPlaneList;
    UT_StringHolder                      myOutputPlane;
    UT_StringHolder                      myCurrentAOV;
};

#endif
