// BSD 3-Clause License

// Copyright (c) 2019, SCALE Lab, Brown University
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.

// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.

// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef __PHY_PHY__
#define __PHY_PHY__
#include <OpenSTA/network/ConcreteNetwork.hh>
#include <PhyKnight/Database/DatabaseHandler.hpp>
#include <PhyKnight/Database/Types.hpp>
#include <PhyKnight/Phy/ProgramOptions.hpp>
#include <PhyKnight/PhyLogger/LogLevel.hpp>
#include <PhyKnight/Sta/DatabaseSta.hpp>
#include <PhyKnight/Transform/PhyTransform.hpp>

#include <unordered_map>

namespace phy
{
class Phy
{
public:
    static Phy& instance();

    virtual Database* database();
    virtual Liberty*  liberty();

    virtual ProgramOptions& programOptions();

    int setLogLevel(const char* level);
    int setLogPattern(const char* pattern);
    int setLogLevel(LogLevel level);

    virtual int readDef(const char* path);
    virtual int readLef(const char* path);
    virtual int readLib(const char* path);

    virtual int writeDef(const char* path);

    int         loadTransforms();
    virtual int runTransform(std::string              transform_name,
                             std::vector<std::string> args);

    int  setupInterpreter(Tcl_Interp* interp);
    int  setupInterpreterReadline();
    void setProgramOptions(int argc, char* argv[]);
    void processStartupProgramOptions();
    int  sourceTclScript(const char* script_path);

    virtual DatabaseHandler* handler() const;

    virtual void printVersion(bool raw_str = false);
    virtual void printUsage(bool raw_str = false);
    virtual void printTransforms(bool raw_str = false);
    ~Phy();

private:
    Phy();
    Database*         db_;
    Liberty*          liberty_;
    sta::DatabaseSta* sta_;
    DatabaseHandler*  db_handler_;

    int initializeDatabase();

    std::unordered_map<std::string, std::shared_ptr<PhyTransform>> transforms_;
    std::unordered_map<std::string, std::string> transforms_versions_;
    std::unordered_map<std::string, std::string> transforms_help_;
    Tcl_Interp*                                  interp_;
    ProgramOptions                               program_options_;
};
} // namespace phy
#endif