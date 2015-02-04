#include "oclint/AbstractASTMatcherRule.h"
#include "oclint/RuleSet.h"

using namespace std;
using namespace clang;
using namespace clang::ast_matchers;
using namespace oclint;

class NoIvarRule : public AbstractASTMatcherRule
{
public:
    virtual const string name() const override
    {
        return "no ivar";
    }

    virtual int priority() const override
    {
        return 3;
    }

    void checkDefinition(const ObjCInterfaceDecl* objcClass) {
        auto ivar = objcClass->all_declared_ivar_begin();
        while (ivar) {
            if (!ivar->getSynthesize()) {
                addViolation(ivar, this);
            }

            ivar = ivar->getNextIvar();
        }
    }

    virtual void callback(const MatchFinder::MatchResult &result) override
    {
        if (auto decl = result.Nodes.getNodeAs<NamedDecl>("decl")) {
            if (ObjCImplementationDecl::classof(decl)) {
                auto objcClass = static_cast<const ObjCImplementationDecl*>(decl);
                checkDefinition(objcClass->getClassInterface());
            }
        }
    }

    virtual void setUpMatcher() override
    {
        addMatcher(namedDecl().bind("decl"));
    }
};

static RuleSet rules(new NoIvarRule());
