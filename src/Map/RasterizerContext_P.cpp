#include "RasterizerContext_P.h"
#include "RasterizerContext.h"

#include "Rasterizer_P.h"
#include "RasterizerSharedContext.h"
#include "RasterizerSharedContext_P.h"
#include "MapObject.h"

#include <SkPathEffect.h>

OsmAnd::RasterizerContext_P::RasterizerContext_P( RasterizerContext* owner_ )
    : owner(owner_)
    , _zoom(ZoomLevel0)
{
}

OsmAnd::RasterizerContext_P::~RasterizerContext_P()
{
}

void OsmAnd::RasterizerContext_P::clear()
{
    // If context has binded shared context, remove all primitives groups
    // that are owned only current context
    if (owner->sharedContext)
    {
        auto& sharedGroups = owner->sharedContext->_p->_sharedPrimitivesGroups[_zoom];
        for(auto& group : _primitivesGroups)
        {
            // Remove reference to this group from shared ones
            sharedGroups.releaseReference(group->mapObject->id, group);
        }
    }
    _primitivesGroups.clear();
    _polygons.clear();
    _polylines.clear();
    _points.clear();

    // If context has binded shared context, remove all symbols groups
    // that are owned only current context
    if (owner->sharedContext)
    {
        auto& sharedGroups = owner->sharedContext->_p->_sharedSymbolGroups[_zoom];
        for(auto& group : _symbolsGroups)
        {
            // Remove reference to this group from shared ones
            sharedGroups.releaseReference(group->mapObject->id, group);
        }
    }
    _symbolsGroups.clear();
    _symbols.clear();
}
