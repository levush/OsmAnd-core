#ifndef _OSMAND_CORE_ONLINE_MAP_RASTER_TILE_PROVIDER_H_
#define _OSMAND_CORE_ONLINE_MAP_RASTER_TILE_PROVIDER_H_

#include <OsmAndCore/stdlib_common.h>

#include <OsmAndCore/QtExtensions.h>
#include <QString>
#include <QDir>

#include <OsmAndCore.h>
#include <OsmAndCore/CommonTypes.h>
#include <OsmAndCore/PrivateImplementation.h>
#include <OsmAndCore/Map/MapTypes.h>
#include <OsmAndCore/Map/IMapBitmapTileProvider.h>

namespace OsmAnd
{
    class OnlineMapRasterTileProvider_P;
    class OSMAND_CORE_API OnlineMapRasterTileProvider : public IMapBitmapTileProvider
    {
        Q_DISABLE_COPY(OnlineMapRasterTileProvider);
    private:
        PrivateImplementation<OnlineMapRasterTileProvider_P> _p;
    protected:
    public:
        OnlineMapRasterTileProvider(const QString& name, const QString& urlPattern,
            const ZoomLevel minZoom = MinZoomLevel, const ZoomLevel maxZoom = MaxZoomLevel,
            const uint32_t maxConcurrentDownloads = 1, const uint32_t providerTileSize = 256,
            const AlphaChannelData alphaChannelData = AlphaChannelData::Undefined);
        virtual ~OnlineMapRasterTileProvider();

        const QString name;
        const QString pathSuffix;
        const QString urlPattern;
        const ZoomLevel minZoom;
        const ZoomLevel maxZoom;
        const uint32_t maxConcurrentDownloads;
        const uint32_t providerTileSize;
        const AlphaChannelData alphaChannelData;

        void setLocalCachePath(const QDir& localCachePath, const bool appendPathSuffix = true);
        const QDir& localCachePath;

        void setNetworkAccessPermission(bool allowed);
        const bool& networkAccessAllowed;

        virtual float getTileDensity() const;
        virtual uint32_t getTileSize() const;

        virtual bool obtainTile(const TileId tileId, const ZoomLevel zoom, std::shared_ptr<const MapTile>& outTile, const IQueryController* const queryController);

        virtual ZoomLevel getMinZoom() const;
        virtual ZoomLevel getMaxZoom() const;
    };
}

#endif // !defined(_OSMAND_CORE_ONLINE_MAP_RASTER_TILE_PROVIDER_H_)