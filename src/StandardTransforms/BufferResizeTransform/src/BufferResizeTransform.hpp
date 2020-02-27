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

#include <OpenPhySyn/Database/DatabaseHandler.hpp>
#include <OpenPhySyn/Database/Types.hpp>
#include <OpenPhySyn/Psn/Psn.hpp>
#include <OpenPhySyn/SteinerTree/SteinerTree.hpp>
#include <OpenPhySyn/Transform/PsnTransform.hpp>
#include <cstring>
#include <memory>
#include <unordered_set>

class BufferResizeTransform : public psn::PsnTransform
{
    class BufferTree
    {
        float capacitance_;
        float required_;
        float wire_capacitance_;
        float wire_delay_;
        float cost_;

    public:
        BufferTree(float cap = 0.0, float req = 0.0, float cost = 0.0)
            : capacitance_(cap), required_(req), cost_(cost)
        {
        }
        float
        capacitance() const
        {
            return capacitance_;
        }
        float
        required() const
        {
            return required_;
        }
        float
        wireCapacitance() const
        {
            return wire_capacitance_;
        }
        float
        wireDelay() const
        {
            return wire_delay_;
        }
        float
        cost() const
        {
            return cost_;
        }
        void
        setCapacitance(float cap)
        {
            capacitance_ = cap;
        }
        void
        setRequired(float req)
        {
            required_ = req;
        }
        void
        setWireCapacitance(float cap)
        {
            wire_capacitance_ = cap;
        }
        void
        setWireDelay(float delay)
        {
            wire_delay_ = delay;
        }
        void
        setCost(float cost)
        {
            cost_ = cost;
        }
    };

    class BufferSolution
    {
        std::vector<std::shared_ptr<BufferTree>> buffer_trees;
        std::shared_ptr<BufferTree>              optimalTree;

    public:
        BufferSolution() : optimalTree(nullptr){};
        BufferSolution(std::unique_ptr<BufferSolution> left,
                       std::unique_ptr<BufferSolution> right)
            : optimalTree(nullptr)
        {
            // TODO merge trees
        }
        void
        addTree(std::shared_ptr<BufferTree>& tree)
        {
            buffer_trees.push_back(std::move(tree));
            // TODO update optimal tree
        }
        std::vector<std::shared_ptr<BufferTree>>&
        bufferTrees()
        {
            return buffer_trees;
        }
        void
        addWireDelays(float total_res, float total_cap)
        {
            // TODO Add wire parasitics
        }

        void
        addLeafTree(std::unordered_set<psn::LibraryCell*>& buffer_lib,
                    std::unordered_set<psn::LibraryCell*>&)
        {
            // TODO Add buffer options
        }
        std::shared_ptr<BufferTree>
        optimalTree()
        {
            // TODO find optimal tree
            return nullptr;
        }

        void
        prune()
        {
            // TODO prune tree
            // TODO update optimal tree
        }
    };

private:
    int  buffer_count_;
    void fixCapacitanceViolations(
        psn::Psn* psn_inst, std::unordered_set<psn::LibraryCell*>& buffer_lib,
        std::unordered_set<psn::LibraryCell*>& inverter_lib,
        bool resize_gates = false, bool use_inverter_pair = false);

    void bufferPin(psn::Psn* psn_inst, psn::InstanceTerm* pin,
                   std::unordered_set<psn::LibraryCell*>& buffer_lib,
                   std::unordered_set<psn::LibraryCell*>& inverter_lib,
                   bool resize_gates = false, bool use_inverter_pair = false);
    std::unique_ptr<BufferSolution>
         bottomUp(psn::Psn* psn_inst, psn::InstanceTerm* pin, psn::SteinerPoint pt,
                  psn::SteinerPoint                      prev,
                  std::unordered_set<psn::LibraryCell*>& buffer_lib,
                  std::unordered_set<psn::LibraryCell*>& inverter_lib,
                  std::shared_ptr<psn::SteinerTree>      st_tree,
                  bool resize_gates = false, bool use_inverter_pair = false);
    void topDown(psn::Psn* psn_inst, psn::InstanceTerm* pin,
                 std::shared_ptr<BufferTree> tree);

    void fixSlewViolations(psn::Psn*                              psn_inst,
                           std::unordered_set<psn::LibraryCell*>& buffer_lib,
                           std::unordered_set<psn::LibraryCell*>& inverter_lib,
                           bool resize_gates      = false,
                           bool use_inverter_pair = false);
    int  fixViolations(psn::Psn* psn_inst, bool fix_cap = true,
                       bool                            fix_slew = true,
                       std::unordered_set<std::string> buffer_lib_names =
                           std::unordered_set<std::string>(),
                       std::unordered_set<std::string> inverter_lib_names =
                           std::unordered_set<std::string>(),
                       bool resize_gates      = false,
                       bool use_inverter_pair = false);

public:
    BufferResizeTransform();

    int run(psn::Psn* psn_inst, std::vector<std::string> args) override;
};

DEFINE_TRANSFORM(
    BufferResizeTransform, "buffer_resize", "1.0.0",
    "Performs several variations of buffering and resizing to fix timing "
    "violations",
    "Usage: transform buffer_resize buffers -all|<set of buffers> [inverters "
    "-all|<set of inverters>] [enable_gate_resize] [enable_inverter_pair]")
