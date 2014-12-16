#ifndef STATICMEMORYALLOCATOR_H
#define STATICMEMORYALLOCATOR_H

#include "../API/IDataStorage.h"

#include <IDataProvider.h>
#include <ImageData.h>
#include <vector>

using namespace OpenCineAPI;

class StaticMemoryAllocator : public IDataStorage
{
    std::vector<OCFrame*> _frameList;
    IDataProvider* _dataProvider;

    // IDataStorage interface
public:
    StaticMemoryAllocator();

    bool ImportFolder(std::string folderPath);

    unsigned int GetFrameCount();
    OCFrame* GetFrame(unsigned int frameNumber);
};

#endif //STATICMEMORYALLOCATOR_H
