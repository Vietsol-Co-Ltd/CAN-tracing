#include "blf_reader.hh"
#include "blf_structs.hh"
#include <algorithm>
#include <vector>

namespace lblf {

blf_reader::blf_reader(const std::string& filename) {
    fileStream.open(filename, std::ios::binary);
    if (!fileStream.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    // Get file size
    fileStream.seekg(0, std::ios::end);
    fileLength = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);

    // Read file statistics
    if (!read_fileStatistics()) {
        throw std::runtime_error("Failed to read file statistics");
    }
}

blf_reader::~blf_reader() {
    if (fileStream.is_open()) {
        fileStream.close();
    }
}

bool blf_reader::fill_deque() {
    if (logcontainer_que.empty()) {
        char buffer[defaultContainerSize];
        fileStream.read(buffer, defaultContainerSize);
        size_t bytesRead = fileStream.gcount();
        if (bytesRead == 0) return false;
        
        logcontainer_que.insert(logcontainer_que.end(), buffer, buffer + bytesRead);
    }
    return true;
}

bool blf_reader::read_baseHeader(blf_struct::BaseHeader& ohb) {
    if (logcontainer_que.size() < sizeof(blf_struct::BaseHeader)) {
        return false;
    }

    // Convert deque to vector for contiguous memory
    std::vector<char> temp(logcontainer_que.begin(), 
                          logcontainer_que.begin() + sizeof(blf_struct::BaseHeader));
    std::memcpy(&ohb, temp.data(), sizeof(blf_struct::BaseHeader));
    
    logcontainer_que.erase(logcontainer_que.begin(), 
                          logcontainer_que.begin() + sizeof(blf_struct::BaseHeader));
    return true;
}

bool blf_reader::read_fileStatistics() {
    fileStream.read(reinterpret_cast<char*>(&fileStat), sizeof(fileStat));
    return fileStream.good();
}

bool blf_reader::next() {
    if (!fill_deque()) {
        return false;
    }

    blf_struct::BaseHeader header;
    if (!read_baseHeader(header)) {
        return false;
    }

    // Read object data
    size_t dataSize = header.objSize - sizeof(blf_struct::BaseHeader);
    if (logcontainer_que.size() < dataSize) {
        return false;
    }

    // Store current object data
    m_currentObject.base_header = header;
    m_currentObject.payload.assign(logcontainer_que.begin(),
                                 logcontainer_que.begin() + dataSize);
    
    // Remove read data from queue
    logcontainer_que.erase(logcontainer_que.begin(),
                          logcontainer_que.begin() + dataSize);

    BaseHeaderRead++;
    return true;
}

lobj blf_reader::data() {
    return m_currentObject;
}

const blf_struct::fileStatistics& blf_reader::getfileStatistics() const {
    return fileStat;
}

size_t blf_reader::getBaseHeadRead() const {
    return BaseHeaderRead;
}

} // namespace lblf 