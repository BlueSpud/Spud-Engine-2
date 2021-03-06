//
//  SFileSystem.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/1/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SFileSystem_hpp
#define SFileSystem_hpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>

#include <dirent.h>
#include <unistd.h>

#include "Engine/SSubsystem.hpp"

// Forward declarations
class SFile;
class SFileSystem;

/******************************************************************************
 *  Definition of path                                                        *
 ******************************************************************************/

class SPath {
    
    friend class SFileSystem;
    friend class SFile;
    
    public:
    
        SPath(std::string relative_path);
    
        const std::string& getExtension() const;
        const std::string& getFilename() const;
        const std::string& getPathAsString() const;
        std::string getPathAsAbsolutePath() const;
    
        bool getIsDirectory() const;
    
        void appendPathComponent(std::string new_component);
        void removeLastPathComponent();
    
    private:
    
        void getFileInfo();
    
        std::string path_str;
        std::string extension;
        std::string filename;
    
        bool is_directory;
};

/******************************************************************************
 *  Definition of generic file                                                *
 ******************************************************************************/

class SFile {
    
    friend class SFileSystem;
    
    public:
    
        bool getNextTokenWithDeliminator(char deliminator, std::string& destination);
        bool bad();
		void endUse();
	
        void read(void* to_place, size_t size);
    
    private:
	
        SFile(const std::string& _path, bool binary = false);
    
        void load(const std::string& _path, bool binary = false);
        void close();
        void reset();
    
        std::ifstream in_stream;
        std::string path;
	
		bool done = false;
	
};

/******************************************************************************
 *  Definition of writable file                                               *
 ******************************************************************************/

class SFileWritable {
	
	friend class SFileSystem;
	
	public:
	
		void close();
	
		template <class T>
		void write(const T& value) { out_stream << value; }
		void write(void* data, size_t size);
	
	private:
	
		SFileWritable(const std::string& _path, bool binary = false);
		std::ofstream out_stream;
		std::string path;
	
};

/******************************************************************************
 *  Definition of file system                                                 *
 ******************************************************************************/

class SFileSystem : public SSubsystem {
    
    public:
    
        // Startup and shutdown functions
        static void startup();
        static void shutdown();
    
        static void getDefaultRootDirectory(std::string argv_0);
        static void setRootDirectory(const SPath& _root_directory);
    
        static SFile* loadFile(const SPath& path, bool binary = false);
		static SFileWritable* openWritableFile(const SPath& path, bool binary = false);
        static void closeFile(SFile* file);
    
        static std::vector<SPath> listDirectory(SPath& path);
        static bool fileExitsAtPath(const SPath& path);
    
        static void writeStringToFile(const SPath& path, const std::string& str);
		static void writeStringToFile(const std::string& path, const std::string& str);
    
    private:
    
        static std::map<size_t, SFile*> loaded_files;
    
};


#endif /* SFileSystem_hpp */
