// Copyright (c) 2017 apertus° Association & contributors
// Project: OpenCine / ProcessingTest
// License: GNU GPL Version 3 (https://www.gnu.org/licenses/gpl-3.0.en.html)

#ifndef IPROCESSINGVIEW_H
#define IPROCESSINGVIEW_H

#include <vector>
#include <string>

#include <QWidget>

#include <Image/OCImage.h>

using namespace OC::DataProvider;

class IProcessingView : public QWidget
{
    Q_OBJECT

public:
	virtual void SetFrame(OCImage& image) = 0;
	virtual void SetThumbnail(unsigned int width, unsigned int height, unsigned char* data) = 0;

    virtual void EnableRendering(bool enable) = 0;

    virtual void SetAvailableDebayerMethods(QStringList debayerMethods) = 0;

signals:
    void OpenRAWFile();
    void DebayerMethodChanged(int index);
    void DumpPNG();
};

#endif //IPROCESSINGVIEW_H
