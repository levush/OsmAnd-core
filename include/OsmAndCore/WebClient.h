#ifndef _OSMAND_CORE_WEB_CLIENT_H_
#define _OSMAND_CORE_WEB_CLIENT_H_

#include <OsmAndCore/stdlib_common.h>
#include <functional>

#include <OsmAndCore/QtExtensions.h>
#include <QString>
#include <QByteArray>
#include <QUrl>
#include <QNetworkReply>

#include <OsmAndCore.h>
#include <OsmAndCore/PrivateImplementation.h>

namespace OsmAnd
{
    class WebClient_P;
    class OSMAND_CORE_API WebClient
    {
        Q_DISABLE_COPY(WebClient);
    public:
        typedef std::function<void(const uint64_t transferredBytes, const uint64_t totalBytes)> RequestProgressCallbackSignature;

        class OSMAND_CORE_API RequestResult
        {
        private:
        protected:
            RequestResult(const QNetworkReply* const networkReply);
        public:
            virtual ~RequestResult();

            const QNetworkReply::NetworkError errorCode;

            virtual bool isSuccessful() const;
        };
        
        class OSMAND_CORE_API HttpRequestResult : public RequestResult
        {
        private:
        protected:
            HttpRequestResult(const QNetworkReply* const networkReply);
        public:
            virtual ~HttpRequestResult();

            const unsigned int httpStatusCode;

        friend class OsmAnd::WebClient_P;
        };

    private:
        PrivateImplementation<WebClient_P> _p;
    protected:
    public:
        WebClient(
            const QString& userAgent = QString::null,
            const unsigned int concurrentRequestsLimit = 1,
            const unsigned int retriesLimit = 1,
            const bool followRedirects = true);
        virtual ~WebClient();

        // "User-Agent" setting:
        static const QString BuiltinUserAgent;
        static QString getDefaultUserAgent();
        static void setDefaultUserAgent(const QString& userAgent);
        QString getUserAgent() const;
        void setUserAgent(const QString& userAgent);

        // Concurrent requests limit:
        unsigned int getConcurrentRequestsLimit() const;
        void setConcurrentRequestsLimit(const unsigned int newLimit);

        // Retries limit:
        unsigned int getRetriesLimit() const;
        void setRetriesLimit(const unsigned int newLimit);

        // Redirects following:
        bool getFollowRedirects() const;
        void setFollowRedirects(const bool followRedirects);

        // Operations:
        QByteArray downloadData(
            const QNetworkRequest& networkRequest,
            std::shared_ptr<const RequestResult>* const requestResult = nullptr,
            const RequestProgressCallbackSignature progressCallback = nullptr);
        QByteArray downloadData(
            const QUrl& url,
            std::shared_ptr<const RequestResult>* const requestResult = nullptr,
            const RequestProgressCallbackSignature progressCallback = nullptr);
        QString downloadString(
            const QNetworkRequest& networkRequest,
            std::shared_ptr<const RequestResult>* const requestResult = nullptr,
            const RequestProgressCallbackSignature progressCallback = nullptr);
        QString downloadString(
            const QUrl& url,
            std::shared_ptr<const RequestResult>* const requestResult = nullptr,
            const RequestProgressCallbackSignature progressCallback = nullptr);
        bool downloadFile(
            const QNetworkRequest& networkRequest,
            const QString& fileName,
            std::shared_ptr<const RequestResult>* const requestResult = nullptr,
            const RequestProgressCallbackSignature progressCallback = nullptr);
        bool downloadFile(
            const QUrl& url,
            const QString& fileName,
            std::shared_ptr<const RequestResult>* const requestResult = nullptr,
            const RequestProgressCallbackSignature progressCallback = nullptr);
    };
}

#endif // !defined(_OSMAND_CORE_WEB_CLIENT_H_)

