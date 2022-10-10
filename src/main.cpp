#include <util/colt_pch.h>
#include <parsing/colt_lexer.h>
#include <ast/colt_expr.h>

using namespace colt;

int main(int argc, const char** argv)
{
  //Populates GlobalArguments
  args::ParseArguments(argc, argv);

  char buffer[2500];
  while (!feof(stdin))
  {
    fgets(buffer, 2500, stdin);
    lang::Lexer lexer = { buffer };

    lang::Token tkn = lexer.get_next_token();
    while (tkn != lang::TKN_EOF)
      tkn = lexer.get_next_token();
  }

  io::PrintMessage("Hello Colt!");
  io::PrintWarning("Hello Colt!");
  io::PrintError("Hello Colt!");
}