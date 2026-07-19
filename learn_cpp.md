# LearnCPP Fast-Teach Prompt

Look at the LearnCPP subchapter page that's currently open. I want to learn it **fast, completely, and in an engaging, practical way**. Do the following:

## 1. Read first
- Read the **entire article body** (the full lesson text, not a skim) before writing anything, so the summary is built from complete content.
- Note every cross-reference to other lessons (e.g. "see lesson X.Y") as you read, because their content is in-scope (see section 4b).

## 2. Teach it fast and stickily
- Start with a one-sentence **"what this whole lesson is really about."**
- Then break out the **core ideas** clearly. For each idea:
  - A plain-language explanation.
  - A tiny **annotated code snippet** where relevant.
  - A **"why it matters"** framing.
- Use **analogies** to make concepts intuitive.
- Prefer clear prose and short snippets over walls of text.

## 3. For each subsection/concept, cover these angles
> Only when they apply — don't force them.
- **When it fails:** the specific situation where this approach breaks or errors out.
- **Why they use THIS:** the reason this feature/approach exists and what problem it solves.
- **Why they use THAT:** the reason the alternative exists and when you'd pick it instead.
- **Real coding gotcha:** the concrete mistake you'd actually hit while coding this — the thing that bites you in practice.

## 4. Guarantee full coverage
- After teaching, give a short **coverage check**:
  - List **every section/heading** on the page, in order.
  - Map each one to where it appeared in the explanation, so nothing was dropped.
- Explicitly call out anything **compressed, deprioritized, or left as an external cross-reference** (links to other lessons).
- Confirm **no concept is missing** — only supporting details/examples may be trimmed.
- Note: you can only see the main article, **not** comments/sidebar/ads.

### 4b. Cross-references are in-scope — teach them, don't defer them
- When the lesson references or leans on a concept developed in another lesson (e.g. "same scope," "differentiation," "ambiguous matches"), **teach the relevant substance inline** — enough to understand the current page — regardless of whether I click the link.
- Give cross-referenced concepts the **same treatment as core ideas**: plain-language explanation, a tiny snippet if useful, the gotcha, and an **⚡ HFT Insight** where relevant.
- You may note that a topic is covered more deeply in another lesson, but **state in a sentence what it is** rather than just linking it.
- **Never** use "I thought it might be taught in another lesson, so I left it out" as a reason to skip content. Assume I will **not** read the cross-references.
- Only keep something as a **pure pointer** if it's genuinely tangential to the current page's ideas — and even then, say what it is in one sentence.
- In the coverage check, **explicitly show where each cross-referenced concept was taught inline**.

## 5. Respect copyright
- Don't reproduce large verbatim chunks of the page — teach it in your own words, with only **very short quotes** if needed.

## 6. HFT remarks
- I'm preparing for **high-frequency trading**.
- Whenever a concept is HFT-relevant (performance, latency, memory layout, determinism, zero-cost abstractions, compile-time vs runtime, cache behavior, etc.), add a clearly marked **⚡ HFT Insight** callout box (blockquote) right where it's relevant, explaining how/why it matters in an HFT context.
- If nothing in the lesson is HFT-relevant, **say so briefly** rather than forcing it.

## 7. End with a 30-second self-check
- 3–5 quick questions (with hidden/collapsible answers) so I can test retention.
