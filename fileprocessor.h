#ifndef FILEPROCESSOR_H
#define FILEPROCESSOR_H

#include <QObject>
#include <QMutex>

#include "videosource.h"
#include "hwdecoderfactory.h"

class FileProcessor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(VideoSource* source READ getSource WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(bool processing READ processing WRITE setProcessing NOTIFY processingChanged)
public:
    explicit FileProcessor(QObject *parent = nullptr);
    ~FileProcessor();

    Q_INVOKABLE void processMedia(const QUrl & input);
    Q_INVOKABLE void stopProcess();

    VideoSource *getSource() const;
    void setSource(VideoSource *source);

    bool processing();
    void setProcessing(bool status);

Q_SIGNALS:
    void sourceChanged();
    void processingChanged();

private:
    void processFile(const QString & input);
    void decodeFile(const QString & input);


    HWDecoder* m_decoder;
    VideoSource* m_source;

    bool m_processing;
    QMutex m_mutex;
};

static QObject *FileProcessorInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new FileProcessor();
}

#endif // FILEPROCESSOR_H
