#include "fileprocessor.h"
#include <QMutexLocker>
#include <QtConcurrent>

FileProcessor::FileProcessor(QObject *parent)
    : QObject(parent),
      m_decoder(nullptr),
      m_source(nullptr),
      m_processing(false)
{
    m_source = new VideoSource(this);
    m_decoder = HWDecoderFactory::createDecoder(this);
    connect(m_decoder, &HWDecoder::frameDecoded, m_source, &VideoSource::setFrame);
}

FileProcessor::~FileProcessor()
{
    disconnect(m_source);
}

VideoSource *FileProcessor::getSource() const
{
    return m_source;
}

void FileProcessor::setSource(VideoSource *source)
{
    if (m_source != source) {
        m_source = source;
        Q_EMIT sourceChanged();
    }
}

void FileProcessor::processFile(const QString & input)
{
    setProcessing(true);
    decodeFile(input);
    setProcessing(false);
}

void FileProcessor::decodeFile(const QString &input)
{
    int video_stream, ret;
    AVPacket packet;

    /* open the input file */
    AVFormatContext * formatCtx = nullptr;
    if (avformat_open_input(&formatCtx, input.toStdString().c_str(), NULL, NULL) != 0) {
        qWarning() << "Cannot open input file" << input;
        return;
    }

    QScopedPointer<AVFormatContext, AVFormatContextDeleter> inputCtx(formatCtx);

    if (avformat_find_stream_info(inputCtx.data(), NULL) < 0) {
        qWarning() << "Cannot find input stream information.";
        return;
    }

    /* find the video stream information */
    ret = av_find_best_stream(inputCtx.data(), AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (ret < 0) {
        qWarning() << "Cannot find a video stream in the input file";
        return;
    }
    video_stream = ret;
    AVCodecParameters* codecParameters = inputCtx->streams[video_stream]->codecpar;

    if (!m_decoder->init(codecParameters)) {
        return;
    }

    if (!m_decoder->open()) {
        return;
    }

    //Decoding loop
    while (processing() && ret >= 0) {
        if ((ret = av_read_frame(inputCtx.data(), &packet)) < 0)
            break;

        if (video_stream == packet.stream_index)
            ret = m_decoder->decode(&packet);

        av_packet_unref(&packet);
    }

    m_decoder->flush();
    m_decoder->close();
}

bool FileProcessor::processing()
{
    QMutexLocker lock(&m_mutex);
    return m_processing;
}

void FileProcessor::setProcessing(bool status)
{
    if (processing() != status) {
        {
            QMutexLocker lock(&m_mutex);
            m_processing = status;
        }

        Q_EMIT processingChanged();
    }
}

void FileProcessor::processMedia(const QUrl &input)
{
    if (processing()) {
        qWarning() << "File Processor is Busy!!";
        return;
    }

    //Call processFile in another thread
    QtConcurrent::run(this, &FileProcessor::processFile, input.toLocalFile());
}

void FileProcessor::stopProcess()
{
    setProcessing(false);
}
