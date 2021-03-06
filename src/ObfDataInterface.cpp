#include "ObfDataInterface.h"

#include "ObfReader.h"
#include "ObfInfo.h"
#include "ObfMapSectionReader.h"
#include "IQueryController.h"

OsmAnd::ObfDataInterface::ObfDataInterface(const QList< std::shared_ptr<const ObfReader> >& obfReaders_)
    : obfReaders(obfReaders_)
{
}

OsmAnd::ObfDataInterface::~ObfDataInterface()
{
}

bool OsmAnd::ObfDataInterface::loadObfFiles( QList< std::shared_ptr<const ObfFile> >* outFiles /*= nullptr*/, const IQueryController* const controller /*= nullptr*/ )
{
    for(const auto& obfReader : constOf(obfReaders))
    {
        if (controller && controller->isAborted())
            return false;

        // Initialize OBF file
        obfReader->obtainInfo();

        if (outFiles)
            outFiles->push_back(obfReader->obfFile);
    }

    return true;
}

bool OsmAnd::ObfDataInterface::loadBasemapPresenceFlag( bool& basemapPresent, const IQueryController* const controller /*= nullptr*/ )
{
    basemapPresent = false;
    for(const auto& obfReader : constOf(obfReaders))
    {
        if (controller && controller->isAborted())
            return false;

        const auto& obfInfo = obfReader->obtainInfo();
        basemapPresent = obfInfo->isBasemap;
        if (basemapPresent)
            break;
    }

    return true;
}

bool OsmAnd::ObfDataInterface::loadMapObjects(
    QList< std::shared_ptr<const OsmAnd::Model::MapObject> >* resultOut, MapFoundationType* foundationOut,
    const AreaI& area31, const ZoomLevel zoom,
    const IQueryController* const controller /*= nullptr*/, const FilterMapObjectsByIdSignature filterById /*= nullptr*/,
    ObfMapSectionReader_Metrics::Metric_loadMapObjects* const metric /*= nullptr*/)
{
    if (foundationOut)
        *foundationOut = MapFoundationType::Undefined;

    // Iterate through all OBF readers
    for(const auto& obfReader : constOf(obfReaders))
    {
        // Check if request is aborted
        if (controller && controller->isAborted())
            return false;

        // Iterate over all map sections of each OBF reader
        const auto& obfInfo = obfReader->obtainInfo();
        for(const auto& mapSection : constOf(obfInfo->mapSections))
        {
            // Check if request is aborted
            if (controller && controller->isAborted())
                return false;

            // Read objects from each map section
            OsmAnd::ObfMapSectionReader::loadMapObjects(obfReader, mapSection, zoom, &area31, resultOut, foundationOut, filterById, nullptr, controller, metric);
        }
    }

    return true;
}
