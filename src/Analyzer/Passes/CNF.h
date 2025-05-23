#pragma once

#include <Analyzer/IQueryTreeNode.h>
#include <Analyzer/Passes/CNFAtomicFormula.h>

#include <Interpreters/Context_fwd.h>

#include <set>

namespace DB::Analyzer
{

class CNF
{
public:
    // Different hash is generated for different order, so we use std::set
    using OrGroup = std::set<CNFAtomicFormula>;
    using AndGroup = std::set<OrGroup>;

    std::string dump() const;

    static constexpr size_t DEFAULT_MAX_GROWTH_MULTIPLIER = 20;
    static constexpr size_t MAX_ATOMS_WITHOUT_CHECK = 200;

    CNF & transformAtoms(std::function<CNFAtomicFormula(const CNFAtomicFormula &)> fn);
    CNF & transformGroups(std::function<OrGroup(const OrGroup &)> fn);

    CNF & filterAlwaysTrueGroups(std::function<bool(const OrGroup &)> predicate);
    CNF & filterAlwaysFalseAtoms(std::function<bool(const CNFAtomicFormula &)> predicate);

    CNF & reduce();

    void appendGroup(const AndGroup & and_group);

    /// Convert "NOT fn" to a single node representing inverse of "fn"
    CNF & pushNotIntoFunctions(const ContextPtr & context);
    CNF & pullNotOutFunctions(const ContextPtr & context);

    static CNFAtomicFormula pushNotIntoFunction(const CNFAtomicFormula & atom, const ContextPtr & context);

    explicit CNF(AndGroup statements_);

    static std::optional<CNF> tryBuildCNF(const QueryTreeNodePtr & node, ContextPtr context, size_t max_growth_multiplier = DEFAULT_MAX_GROWTH_MULTIPLIER);
    static CNF toCNF(const QueryTreeNodePtr & node, ContextPtr context, size_t max_growth_multiplier = DEFAULT_MAX_GROWTH_MULTIPLIER);

    QueryTreeNodePtr toQueryTree() const;

    const auto & getStatements() const
    {
        return statements;
    }
private:
    AndGroup statements;
};

}
