:: dont call this file manually, this file is called thru visual studio on compilation
@echo off
%cd%\proto\protoc.exe --cpp_out=%cd%\src\ --proto_path %cd%\proto appearances.proto shared.proto
PAUSE