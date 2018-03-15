//===--- ConstexprFunctionCheck.cpp - clang-tidy---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "ConstexprFunctionCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <iostream>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace performance {

namespace
{
AST_MATCHER(VarDecl, isAsm) { return Node.hasAttr<clang::AsmLabelAttr>(); }
//AST_MATCHER(VarDecl, isInitialized) { return varDecl(hasInitializer(anything())); }
AST_MATCHER(FunctionDecl, isReturnTypeLiteral) { return Node.getAsFunction()->getReturnType().getTypePtr()->isLiteralType(Node.getASTContext()); }
AST_MATCHER(VarDecl, isLiteral) { return Node.getType().getTypePtr()->isLiteralType(Node.getASTContext()); }
const ast_matchers::internal::VariadicDynCastAllOfMatcher<Decl,
                FileScopeAsmDecl>
fileScopeAsmDecl;
} // namespace

void ConstexprFunctionCheck::registerMatchers(MatchFinder *Finder) {
  //Finder->addMatcher(functionDecl().bind("x"), this);

  /*Finder->addMatcher(asmStmt().bind("asm-stmt"), this);
  Finder->addMatcher(fileScopeAsmDecl().bind("asm-file-scope"), this);
  Finder->addMatcher(varDecl(isAsm()).bind("asm-var"), this);*/
  Finder->addMatcher(functionDecl(allOf(unless(isConstexpr()),
                                        isReturnTypeLiteral(),
                                        unless(anyOf(hasDescendant(asmStmt()),
                                                     hasDescendant(varDecl(isAsm())),
                                                     hasDescendant(gotoStmt()),
                                                     hasDescendant(labelStmt()),
                                                     hasDescendant(cxxTryStmt()),
                                                     hasDescendant(varDecl(hasStaticStorageDuration())),
                                                     hasDescendant(varDecl(hasThreadStorageDuration())),
                                                     hasDescendant(varDecl(unless(isLiteral()))),
                                                     hasDescendant(varDecl(hasInitializer(anything())))
                                        )))).bind("function"), this);


}

/*an asm declaration
a goto statement
a statement with a label other than case and default
a try-block
a definition of a variable of non-literal type
a definition of a variable of static or thread storage duration
a definition of a variable for which no initialization is performed.*/

/*
 * 1) the compound statement of the constructor body must satisfy the constraints for the body of a constexpr function
   2) for the constructor of a class or struct, every base class sub-object and every non-variant non-static data member must be initialized.
      If the class is a union-like class, for each of its non-empty anonymous union members, exactly one variant member must be initialized
   3) for the constructor of a non-empty union, exactly one non-static data member must be initialized
   4) every constructor selected to initializing non-static members and base class must be a constexpr constructor.*/
bool ConstexprClassCtorBodyCheck(const CXXConstructorDecl& body)
{
  return true;
}

void ConstexprFunctionCheck::check(const MatchFinder::MatchResult &Result)
{
  const auto& MatchedDecl = *Result.Nodes.getNodeAs<FunctionDecl>("function");

  /*if (MatchedDecl.getParent()->getNumVBases() > 0 ||
      std::string(MatchedDecl.getBody()->getStmtClassName()) == "CXXTryStmt")
  {
    return;
  }*/

  // Check parameters for Literals only
  for(const auto& param : MatchedDecl.parameters())
  {
    const auto paramTypePtr = param->getType().getTypePtr();
    if(!paramTypePtr->isLiteralType(MatchedDecl.getASTContext()))
    {
      return;
    }
  }

  /*if(MatchedDecl.isDefaultConstructor() ||
     MatchedDecl.isDeleted() ||
     ConstexprClassCtorBodyCheck(MatchedDecl))*/
  {
    diag(MatchedDecl.getLocation(), "constructor %0 isn't constexpr")
            << &MatchedDecl
            << FixItHint::CreateInsertion(MatchedDecl.getLocation(), "constexpr ");
  }
}

} // namespace performance
} // namespace tidy
} // namespace clang
