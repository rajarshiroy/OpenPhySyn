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

#include "PinSwapTransform.hpp"
#include <OpenPhySyn/PsnLogger/PsnLogger.hpp>
#include <OpenPhySyn/Utils/PsnGlobal.hpp>
#include <OpenSTA/dcalc/ArcDelayCalc.hh>
#include <OpenSTA/dcalc/GraphDelayCalc.hh>
#include <OpenSTA/liberty/TimingArc.hh>
#include <OpenSTA/liberty/TimingModel.hh>
#include <OpenSTA/liberty/TimingRole.hh>
#include <OpenSTA/search/Corner.hh>

#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>

using namespace psn;

PinSwapTransform::PinSwapTransform() : swap_count_(0)
{
}

int
PinSwapTransform::powerPinSwap(psn::Psn* psn_inst)
{
    PsnLogger::instance().error(
        "Pin-Swapping for power optimization is not supported yet.");
    return swap_count_;
}
int
PinSwapTransform::timingPinSwap(psn::Psn* psn_inst)
{
    PsnLogger&       logger  = PsnLogger::instance();
    DatabaseHandler& handler = *(psn_inst->handler());
    auto             cp      = handler.criticalPath();
    // auto             bp      = handler.bestPath();
    std::reverse(cp.begin(), cp.end());

    for (auto& point : cp)
    {

        auto pin      = std::get<0>(point);
        auto is_rise  = std::get<1>(point);
        auto inst     = handler.instance(pin);
        auto lib_cell = handler.libraryCell(inst);
        auto ap_index = std::get<4>(point);

        if (!handler.isInput(pin))
        {
            continue;
        }
        auto input_pins  = handler.inputPins(inst);
        auto output_pins = handler.outputPins(inst);
        if (input_pins.size() < 2 || output_pins.size() != 1)
        {
            continue;
        }
        auto out_pin = output_pins[0];
        for (auto& in_pin : input_pins)
        {
            if (in_pin != pin && handler.isCommutative(in_pin, pin))
            {
                float current_arrival =
                    handler.arrival(out_pin, ap_index, is_rise);
                handler.swapPins(pin, in_pin);
                float new_arrival = handler.arrival(out_pin, ap_index, is_rise);
                if (new_arrival < current_arrival)
                {
                    logger.debug("Accepted Swap: {} <-> {}", handler.name(pin),
                                 handler.name(in_pin));
                    swap_count_++;
                }
                else
                {
                    handler.swapPins(pin, in_pin);
                }
            }
        }
    }
    return swap_count_;
}

bool
PinSwapTransform::isNumber(const std::string& s)
{
    std::istringstream iss(s);
    float              f;
    iss >> std::noskipws >> f;
    return iss.eof() && !iss.fail();
}
int
PinSwapTransform::run(Psn* psn_inst, std::vector<std::string> args)
{
    bool power_opt = false;
    if (args.size() > 1)
    {
        PsnLogger::instance().error(help());
        return -1;
    }
    else if (args.size())
    {
        std::transform(args[0].begin(), args[0].end(), args[0].begin(),
                       ::tolower);
        if (args[0] == "true" || args[0] == "1")
        {
            power_opt = true;
        }
        else if (args[0] == "false" || args[0] == "0")
        {
            power_opt = false;
        }
        else
        {
            PsnLogger::instance().error(help());
            return -1;
        }
    }
    swap_count_ = 0;
    if (power_opt)
    {
        return powerPinSwap(psn_inst);
    }
    else
    {
        return timingPinSwap(psn_inst);
    }
}
