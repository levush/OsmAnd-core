#include "OnlineMapRasterTileProvider_P.h"
#include "OnlineMapRasterTileProvider.h"

#include <cassert>

#include "QtExtensions.h"
#include <QCoreApplication>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>

#include <SkStream.h>
#include <SkImageDecoder.h>

#include "Logging.h"

OsmAnd::OnlineMapRasterTileProvider_P::OnlineMapRasterTileProvider_P(OnlineMapRasterTileProvider* owner_)
    : owner(owner_)
    , _networkAccessAllowed(true)
{
}

OsmAnd::OnlineMapRasterTileProvider_P::~OnlineMapRasterTileProvider_P()
{
}

bool OsmAnd::OnlineMapRasterTileProvider_P::obtainTile(const TileId tileId, const ZoomLevel zoom, std::shared_ptr<const MapTile>& outTile, const IQueryController* const queryController)
{
    // Check provider can supply this zoom level
    if (zoom > owner->maxZoom || zoom < owner->minZoom)
    {
        outTile.reset();
        return true;
    }

    // Check if requested tile is already being processed, and wait until that's done
    // to mark that as being processed.
    lockTile(tileId, zoom);

    // Check if requested tile is already in local storage.
    const auto tileLocalRelativePath =
        QString::number(zoom) + QDir::separator() +
        QString::number(tileId.x) + QDir::separator() +
        QString::number(tileId.y) + QLatin1String(".tile");
    QFileInfo localFile;
    {
        QMutexLocker scopedLocker(&_localCachePathMutex);
        localFile.setFile(_localCachePath.absoluteFilePath(tileLocalRelativePath));
    }
    if (localFile.exists())
    {
        // Since tile is in local storage, it's safe to unmark it as being processed
        unlockTile(tileId, zoom);

        // If local file is empty, it means that requested tile does not exist (has no data)
        if (localFile.size() == 0)
        {
            outTile.reset();
            return true;
        }

        auto bitmap = new SkBitmap();
        SkFILEStream fileStream(qPrintable(localFile.absoluteFilePath()));
        if (!SkImageDecoder::DecodeStream(&fileStream, bitmap, SkBitmap::Config::kNo_Config, SkImageDecoder::kDecodePixels_Mode))
        {
            LogPrintf(LogSeverityLevel::Error, "Failed to decode tile file '%s'", qPrintable(localFile.absoluteFilePath()));

            delete bitmap;

            return false;
        }

        assert(bitmap->width() == bitmap->height());
        assert(bitmap->width() == owner->providerTileSize);

        // Return tile
        auto tile = new MapBitmapTile(bitmap, owner->alphaChannelData);
        outTile.reset(tile);
        return true;
    }

    // Since tile is not in local cache (or cache is disabled, which is the same),
    // the tile must be downloaded from network:

    // If network access is disallowed, return failure
    if (!_networkAccessAllowed)
    {
        // Before returning, unlock tile
        unlockTile(tileId, zoom);

        return false;
    }

    // Perform synchronous download
    auto tileUrl = owner->urlPattern;
    tileUrl
        .replace(QLatin1String("${osm_zoom}"), QString::number(zoom))
        .replace(QLatin1String("${osm_x}"), QString::number(tileId.x))
        .replace(QLatin1String("${osm_y}"), QString::number(tileId.y));
    std::shared_ptr<const WebClient::RequestResult> requestResult;
    const auto& downloadResult = _downloadManager.downloadData(QUrl(tileUrl), &requestResult);

    // Ensure that all directories are created in path to local tile
    localFile.dir().mkpath(QLatin1String("."));

    // If there was error, check what the error was
    if (!requestResult->isSuccessful())
    {
        const auto httpStatus = std::dynamic_pointer_cast<const WebClient::HttpRequestResult>(requestResult)->httpStatusCode;

        LogPrintf(LogSeverityLevel::Warning, "Failed to download tile from %s (HTTP status %d)", qPrintable(tileUrl), httpStatus);

        // 404 means that this tile does not exist, so create a zero file
        if (httpStatus == 404)
        {
            // Save to a file
            QFile tileFile(localFile.absoluteFilePath());
            if (tileFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
            {
                tileFile.close();

                // Unlock the tile
                unlockTile(tileId, zoom);
                return true;
            }
            else
            {
                LogPrintf(LogSeverityLevel::Error, "Failed to mark tile as non-existent with empty file '%s'", qPrintable(localFile.absoluteFilePath()));

                // Unlock the tile
                unlockTile(tileId, zoom);
                return false;
            }
        }

        // Unlock the tile
        unlockTile(tileId, zoom);
        return false;
    }

    // Obtain all data
#if OSMAND_DEBUG
    LogPrintf(LogSeverityLevel::Info, "Downloaded tile from %s", qPrintable(tileUrl));
#endif

    // Save to a file
    QFile tileFile(localFile.absoluteFilePath());
    if (tileFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        tileFile.write(downloadResult);
        tileFile.close();

#if OSMAND_DEBUG
        LogPrintf(LogSeverityLevel::Info, "Saved tile from %s to %s", qPrintable(tileUrl), qPrintable(localFile.absoluteFilePath()));
#endif
    }
    else
        LogPrintf(LogSeverityLevel::Error, "Failed to save tile to '%s'", qPrintable(localFile.absoluteFilePath()));

    // Unlock tile, since local storage work is done
    unlockTile(tileId, zoom);

    // Decode in-memory
    auto bitmap = new SkBitmap();
    if (!SkImageDecoder::DecodeMemory(downloadResult.constData(), downloadResult.size(), bitmap, SkBitmap::Config::kNo_Config, SkImageDecoder::kDecodePixels_Mode))
    {
        LogPrintf(LogSeverityLevel::Error, "Failed to decode tile file from '%s'", qPrintable(tileUrl));

        delete bitmap;

        return false;
    }

    assert(bitmap->width() == bitmap->height());
    assert(bitmap->width() == owner->providerTileSize);

    // Return tile
    auto tile = new MapBitmapTile(bitmap, owner->alphaChannelData);
    outTile.reset(tile);
    return true;
}

void OsmAnd::OnlineMapRasterTileProvider_P::lockTile(const TileId tileId, const ZoomLevel zoom)
{
    QMutexLocker scopedLocker(&_tilesInProcessMutex);

    while(_tilesInProcess[zoom].contains(tileId))
        _waitUntilAnyTileIsProcessed.wait(&_tilesInProcessMutex);

    _tilesInProcess[zoom].insert(tileId);
}

void OsmAnd::OnlineMapRasterTileProvider_P::unlockTile(const TileId tileId, const ZoomLevel zoom)
{
    QMutexLocker scopedLocker(&_tilesInProcessMutex);

    _tilesInProcess[zoom].remove(tileId);

    _waitUntilAnyTileIsProcessed.wakeAll();
}
