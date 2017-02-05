//
//  SFileSystem.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/1/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SFileSystem.hpp"

/******************************************************************************
 *  Global declarations                                                       *
 ******************************************************************************/

std::map<unsigned long, SFile*> SFileSystem::loaded_files;
std::hash<std::string> SFileSystem::hasher;

std::string root_directory;

/******************************************************************************
 *  Implementation for the path class                                         *
 ******************************************************************************/

SPath::SPath(std::string relative_path) {
    
    // Create the path based on the prefix and the relative path
    path_str = relative_path;
    
    // Get extension, etc
    getFileInfo();
    
}

void SPath::appendPathComponent(std::string new_component) {
    
    // Add the new copmponent on
    path_str = path_str + new_component;
    
    // Get extension, etc
    getFileInfo();
    
}

void SPath::removeLastPathComponent() {
    
    std::stringstream stream = std::stringstream(path_str);
    std::string second_last_path_component;
    std::string last_path_component;
    
    // Get the last path component
    while (true) {
        
        second_last_path_component = last_path_component;
        if (!std::getline(stream, last_path_component, '/'))
            break;
        
    }
    
    // If we are a directory, we are going to go up a directory
    if (is_directory)
        last_path_component = "/" + second_last_path_component;
    
    path_str.replace(path_str.length() - last_path_component.length(), last_path_component.length(), "");
    
    // Get the new info
    getFileInfo();
    
}

void SPath::getFileInfo() {
    
    // Extract the file name and the prefix
    std::stringstream stream = std::stringstream(path_str);
    std::string last_path_component;
    
    // Get the last path component
    while (std::getline(stream, last_path_component, '/')) {}
    std::stringstream last_path_component_s = std::stringstream(last_path_component);

    
    // Get the extension adn the file name
    std::getline(last_path_component_s, filename, '.');
    std::getline(last_path_component_s, extension, '\n');
    
    if (!filename.length() && !extension.length())
        is_directory = true;
    else is_directory = false;
    
}

const std::string& SPath::getExtension() const { return extension; }
const std::string& SPath::getFilename() const { return filename; }
const std::string& SPath::getPathAsString() const { return path_str; }
std::string SPath::getPathAsAbsolutePath() const { return root_directory + path_str; }


bool SPath::getIsDirectory() const { return is_directory; }

/******************************************************************************
 *  Implementation for the file class                                         *
 ******************************************************************************/

SFile::SFile() { /* default constructor, nothing*/ }

SFile::SFile(const std::string& _path, bool binary) {
    
    // Shortcut to load
    load(_path, binary);

}

void SFile::load(const std::string& _path, bool binary) {
    
    // If we requested to load it from binary, use the binary flag
    if (binary)
        in_stream = std::ifstream(_path, std::ios::binary);
    else
        in_stream = std::ifstream(_path);
    
    // Save the path
    path = _path;
    
}

void SFile::close() {
    
    // Close the stream if its open
    if (in_stream.is_open())
        in_stream.close();
}

void SFile::reset() {
    
    // Go back to the beginning of the file in case we want to use it again
    in_stream.clear();
    in_stream.seekg(0, std::ios::beg);
    
}

bool SFile::getNextTokenWithDeliminator(char deliminator, std::string& destination) {
    
    if (!bad()) {
    
        std::string destination_temp;
    
        // Make an attempt to use the stream
        if (std::getline(in_stream, destination_temp, deliminator)) {
        
            destination = destination_temp;
            return true;
        
        } else {
            
            reset();
            return false;
        }
        
    }
    
    return false;
    
}

bool SFile::bad() {
    
    // If it is valid, just get the streams badness
    if (in_stream)
        return in_stream.bad();
    
    return false;
}

void SFile::read(void* to_place, std::streamsize size) {
    
    // Read from the stream
    in_stream.read((char*)to_place, size);
    
}

/******************************************************************************
 *  Implementation for the file system class                                  *
 ******************************************************************************/

void SFileSystem::startup() {
    
    // No startup required for file system
    SLog::verboseLog(SVerbosityLevel::Debug, "SFileSystem startup");
}

void SFileSystem::shutdown() {

    // Delete all the loaded files
    for (std::map<unsigned long, SFile*>::iterator i = loaded_files.begin(); i != loaded_files.end(); i++) {
        
        SFile* file = i->second;
        
        if (file) {
        
            // Close the file and delete it from memory
            file->close();
            delete file;
            
        }
        
    }

    SLog::verboseLog(SVerbosityLevel::Debug, "SFileSystem shutdown");

}

void SFileSystem::getDefaultRootDirectory(std::string argv_0) {
    
    // Calculate root path
    SPath root_filesystem_path = SPath(argv_0);
    
    #ifdef __APPLE__
    
        root_filesystem_path.removeLastPathComponent();
        root_filesystem_path.removeLastPathComponent();
    
    #endif
    
    // Append the resources folder
    root_filesystem_path.appendPathComponent("Resources/");
    
    setRootDirectory(root_filesystem_path);
    
}

void SFileSystem::setRootDirectory(const SPath& _root_directory) {
    
    // Save the new root directory
    if (_root_directory.is_directory)
        root_directory = _root_directory.path_str;
    
}

SFile* SFileSystem::loadFile(const SPath& path, bool binary) {
    
    // We dont even bother with directories, they cant be loaded anyways
    if (!path.is_directory) {
    
        // Get the hash and check if we have it
        unsigned long hash = hasher(path.path_str);
    
        if (!loaded_files.count(hash)) {
    
            // Create a new file, give the root directory to it
            std::string full_path = root_directory + path.path_str;
            SFile* new_file = new SFile(full_path, binary);
    
            // Make sure that we are good
            if (fileExitsAtPath(path)) {
        
                loaded_files[hash] = new_file;
            
                return new_file;
        
            }
            
            // Get rid of it if we didnt need it
            delete new_file;
        
        }
        
        // Return the already loaded file
        return loaded_files[hash];
        
    }
    
    return nullptr;
    
}

void SFileSystem::closeFile(SFile* file) {
    
    // Find the file and delete it
    std::map<unsigned long, SFile*>::iterator i;
    for (i = loaded_files.begin(); i != loaded_files.end(); i++) {
        
        if (i->second == file)
            break;
        
    }

    // Get rid of it
    file->close();
    delete file;
    loaded_files.erase(i);
    
}

std::vector<SPath> SFileSystem::listDirectory(SPath& path) {
    
    std::vector<SPath>file_paths;
    
    if (path.is_directory) {
        
        // Open up the directory
        DIR* directory = opendir((root_directory + path.path_str).c_str());
        struct dirent *entry;
        
        // Grab the filenames of anything we find
        while ((entry = readdir(directory))) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            
                SPath new_path = SPath(path.path_str + entry->d_name);
                file_paths.push_back(new_path);
                
            }
        }
    
    }
    
    return file_paths;
        
}

bool SFileSystem::fileExitsAtPath(const SPath& path) {
    
    // Check if it exists
    if (access((root_directory + path.path_str).c_str(), F_OK ) != -1)
        return true;
    return false;
    
}

void SFileSystem::writeStringToFile(const SPath& path, const std::string& str) { writeStringToFile(path.getPathAsAbsolutePath(), str); }

void SFileSystem::writeStringToFile(const std::string& path, const std::string& str) {
	
	// Write the contents of a string out to a file
	std::ofstream output_stream = std::ofstream(path);
	
	if (!output_stream.bad())
		output_stream << str;
	
	
	// Close it
	output_stream.close();
	
}
