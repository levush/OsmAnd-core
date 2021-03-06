#ifndef _OSMAND_CORE_Q_FILE_DEVICE_INPUT_STREAM_H_
#define _OSMAND_CORE_Q_FILE_DEVICE_INPUT_STREAM_H_

#include <memory>

#include <OsmAndCore/QtExtensions.h>
#include <QFileDevice>

#include <OsmAndCore.h>
#include <google/protobuf/io/zero_copy_stream.h>

namespace OsmAnd
{
    namespace gpb = google::protobuf;

    /**
    Implementation of input stream for Google Protobuf via QFileDevice with memory mapping
    */
    class OSMAND_CORE_API QFileDeviceInputStream : public gpb::io::ZeroCopyInputStream
    {
    private:
        GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(QFileDeviceInputStream);

        //! Pointer to I/O device
        const std::shared_ptr<QFileDevice> _file;

        //! File size
        const qint64 _fileSize;

        //! Pointer to mapped memory
        uint8_t* _mappedMemory;

        //! Memory window size
        const size_t _memoryWindowSize;

        //! Current position
        qint64 _currentPosition;

        //! Was initially opened?
        const bool _wasInitiallyOpened;

        //! Original open-mode
        const QIODevice::OpenMode _originalOpenMode;

        //! Should close on destruction?
        bool _closeOnDestruction;

        //! Constants
        enum {
            DefaultMemoryWindowSize = 16 * 1024 * 1024, // 16Mb
        };
    protected:
    public:
        //! Ctor
        QFileDeviceInputStream(const std::shared_ptr<QFileDevice>& file, const size_t memoryWindowSize = DefaultMemoryWindowSize);

        //! Dtor
        virtual ~QFileDeviceInputStream();

        virtual bool Next(const void** data, int* size);
        virtual void BackUp(int count);
        virtual bool Skip(int count);
        virtual gpb::int64 ByteCount() const;
    };

} // namespace OsmAnd

#endif // !defined(_OSMAND_CORE_Q_FILE_DEVICE_INPUT_STREAM_H_)
