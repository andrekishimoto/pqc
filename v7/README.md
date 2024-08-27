<a name="portugues"></a>
# PQC (v7)

[(For English, see below)](#english)

- Implementação da checagem de erro quando há mais de um ponto decimal em uma sequência de dígitos no tokenizer.
- Uso de epsilon na comparação de `double`.
- Melhorias no parser para feedback do interpretador.
- `struct ktParser` movida do `.h` para `.c`.
- REPL agora está no interpretador e a `main()` só chama `ktInterpreterRun()`.
- Limpeza do código e algumas refatorações.
- Inclusão dos comandos `CLEAR` e `EXIT`.
- Compilação no Linux e criação do `makefile` do projeto.

## Licença

Copyright &copy; 2024 André Kishimoto.

Projeto licenciado sob [Apache License 2.0](https://choosealicense.com/licenses/apache-2.0/).

---
---
<a name="english"></a>
# PQC (v7)

[(Para português, veja acima)](#portugues)

- Implemented error checking for more than one decimal point in a digit sequence in the tokenizer.
- Using epsilon for `double` comparison.
- Improved the parser for better feedback to the interpreter.
- Moved `struct ktParser` from `.h` to `.c` file.
- REPL is now in the interpreter, and `main()` only calls `ktInterpreterRun()`.
- Code cleanup and some refactoring.
- Added `CLEAR` and `EXIT` commands.
- Compiled on Linux and created the project's `makefile`.

## License

Copyright &copy; 2024 André Kishimoto.

Project licensed under the [Apache License 2.0](https://choosealicense.com/licenses/apache-2.0/).

