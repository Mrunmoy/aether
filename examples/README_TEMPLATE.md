# Example README Template

Use this template when adding or rewriting an example README.

The goal is not to document everything. The goal is to get a reader from
"what is this?" to "I ran it and understand the moving parts."

## Required Sections

```markdown
# Example Name

One sentence describing what the example demonstrates.

## What You'll Learn
- concept 1
- concept 2
- concept 3

## Prerequisites
- supported platform(s)
- required tools or dependencies
- whether this starts from the repo root, an SDK tarball, or firmware tree

## Files That Matter
| File | Why it matters |
|------|----------------|

## Step 1: Read the IDL
If the example uses IDL:
- summarize the service, methods, notifications, and shared types

If it does not use IDL:
- say that explicitly and explain what replaces it

## Step 2: Generate Code
If the example uses code generation:
- show the exact command
- list the generated outputs
- explicitly say whether the generated files are already checked in, so the
  reader knows whether regeneration is required for a first run

If it does not:
- say that explicitly

## Step 3: Review the User Code
- what the server/device implements
- what the client/dashboard implements
- what the runtime handles for you

## Build
Exact commands, including working directory context.

## Run
Use separate terminal labels when the server blocks:

    # Terminal 1
    ...

    # Terminal 2
    ...

## Expected Output
Show the main lines a user should expect to see.

## What Just Happened
2–4 sentences connecting the files, generated code, and runtime behavior.

## What To Modify Next
- one or two safe experiments

## Related Examples
- where to go next in the learning path
```

## Style Rules

- Prefer exact commands over pseudocode.
- Always say which directory commands run from.
- State platform restrictions near the top, not at the end.
- Be explicit about generated code versus user-written code.
- Say whether `python3` can be replaced with `py -3` on Windows if that is the
  intended equivalent command.
- Do not assume the reader already knows the runtime classes.
