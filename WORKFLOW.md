# C-to-C++ conversion workflow

Use a small-loop approach so every C++ change has a clear purpose and is easy
to verify.

1. Choose one small C construct or responsibility to convert.
2. Read the surrounding code and describe the current behavior first.
3. Decide on the C++ equivalent, keeping behavior unchanged.
4. Change one representative place only.
5. Build and run the program to check that it still behaves as expected.
6. Explain the pattern, then apply it to similar places only after the user is
   comfortable doing so (or explicitly asks for implementation help).

## Suggested conversion order

- Make sure the C++ build works reliably.
- Replace C headers and C-style casts where a direct C++ equivalent is clear.
- Introduce `constexpr`, references, and standard-library types where they
  improve clarity without changing the game design.
- Group related game state into small structs/classes only after the existing
  data flow is understood.
- Move ownership and cleanup toward RAII gradually (for example, SDL resource
  wrappers), verifying each step.
- Consider gameplay fixes or new features after the conversion is stable.

## Tutor handoff format

For a question about a conversion, prefer an answer in this shape:

- What the selected C code currently does.
- One C++ approach and why it fits here.
- A small example for that one location.
- The matching locations the user can update using the same pattern.
- A build/run check to perform afterward.

Avoid broad mechanical refactors: learning and preserving behavior take
priority over speed.
