#!/usr/bin/env python3
"""Generate a styled benchmark dashboard HTML from Google Benchmark JSON output.

Usage:
    python3 tools/gen_bench_page.py benchmark_result.json > index.html
"""
import json
import sys
from datetime import datetime, timezone


def load_results(path: str) -> dict:
    with open(path) as f:
        return json.load(f)


def format_time(ns: float) -> str:
    if ns < 1000:
        return f"{ns:.2f} ns"
    elif ns < 1_000_000:
        return f"{ns / 1000:.2f} µs"
    else:
        return f"{ns / 1_000_000:.2f} ms"


def format_rate(val: float, unit: str) -> str:
    if val >= 1e9:
        return f"{val / 1e9:.2f} G{unit}"
    elif val >= 1e6:
        return f"{val / 1e6:.2f} M{unit}"
    elif val >= 1e3:
        return f"{val / 1e3:.2f} K{unit}"
    return f"{val:.2f} {unit}"


def to_ns(b: dict) -> float:
    t = b.get("real_time", 0)
    u = b.get("time_unit", "ns")
    if u == "us":
        t *= 1000
    elif u == "ms":
        t *= 1_000_000
    return t


def extract_param(name: str) -> str:
    """Extract the /N parameter from a benchmark name."""
    if "/" in name:
        return name.rsplit("/", 1)[1]
    return ""


def categorize(name: str) -> str:
    if "WriteFrame" in name or "ReadFrame" in name or "Peek" in name or "Pipeline" in name or "RoundTrip" in name:
        return "Ring I/O"
    elif "Call" in name or "Connect" in name or "Parallel" in name:
        return "IPC Round-Trip"
    elif "Notify" in name or "Broadcast" in name:
        return "Notifications"
    return "Other"


def ops_per_sec(time_ns: float) -> float:
    if time_ns <= 0:
        return 0
    return 1e9 / time_ns


def main():
    if len(sys.argv) < 2:
        print("Usage: gen_bench_page.py <benchmark_result.json>", file=sys.stderr)
        sys.exit(1)

    data = load_results(sys.argv[1])
    ctx = data.get("context", {})
    benchmarks = data.get("benchmarks", [])

    host = ctx.get("host_name", "unknown")
    cpus = ctx.get("num_cpus", "?")
    mhz = ctx.get("mhz_per_cpu", 0)
    now = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%S+00:00")

    # Compute hero stats
    call_benchmarks = [b for b in benchmarks if "CallLatency" in b["name"]]
    ring_benchmarks = [b for b in benchmarks if "WriteFrame" in b["name"]]
    notify_benchmarks = [b for b in benchmarks if "NotifyLatency" in b["name"]]

    best_call_ns = min((to_ns(b) for b in call_benchmarks), default=0)
    best_ring_ns = min((to_ns(b) for b in ring_benchmarks), default=0)
    best_notify_ns = min((to_ns(b) for b in notify_benchmarks), default=0)

    # Compute throughput from iterations_per_second
    all_ns = [to_ns(b) for b in benchmarks if to_ns(b) > 0]
    best_ops = max((ops_per_sec(ns) for ns in all_ns), default=0) if all_ns else 0

    # Group by category
    categories: dict[str, list] = {}
    for b in benchmarks:
        cat = categorize(b["name"])
        categories.setdefault(cat, []).append(b)

    # Build table rows
    rows_by_cat: dict[str, list] = {}
    for cat, items in categories.items():
        rows = []
        for b in items:
            name = b["name"].split("/")[0].replace("BM_", "")
            param = extract_param(b["name"])
            time_ns = to_ns(b)
            ops = ops_per_sec(time_ns)

            # Param column interpretation
            param_label = ""
            if param:
                if "Parallel" in b["name"] or "Broadcast" in b["name"]:
                    param_label = f"{param} clients"
                else:
                    param_label = f"{param} B"

            rows.append((name, param_label, format_time(time_ns), format_rate(ops, "op/s")))
        rows_by_cat[cat] = rows

    def table_html(cat_name: str, rows: list) -> str:
        h = f'    <h2>{cat_name}</h2>\n'
        h += '    <table>\n      <thead>\n'
        h += '        <tr><th>Benchmark</th><th>Param</th><th>Latency</th><th>Throughput</th></tr>\n'
        h += '      </thead>\n      <tbody>\n'
        for name, param, time, ops in rows:
            h += f'        <tr><td>{name}</td><td>{param}</td><td>{time}</td><td>{ops}</td></tr>\n'
        h += '      </tbody>\n    </table>\n'
        return h

    tables = ""
    for cat_order in ["Ring I/O", "IPC Round-Trip", "Notifications", "Other"]:
        if cat_order in rows_by_cat:
            tables += table_html(cat_order, rows_by_cat[cat_order])

    html = f"""<!doctype html>
<html>
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>Aether IPC — Benchmark Dashboard</title>
  <style>
    :root {{
      --bg: #05070d;
      --panel: rgba(12, 18, 35, 0.72);
      --panel2: rgba(8, 12, 24, 0.72);
      --text: #e6f1ff;
      --muted: rgba(230, 241, 255, 0.65);
      --line: rgba(0, 255, 209, 0.18);
      --line2: rgba(102, 204, 255, 0.16);
      --accent: #00ffd1;
      --accent2: #66ccff;
      --shadow: 0 10px 30px rgba(0,0,0,0.55);
      --radius: 14px;
    }}

    html, body {{ height: 100%; }}

    body {{
      font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace;
      margin: 0;
      color: var(--text);
      background:
        radial-gradient(1200px 600px at 20% 0%, rgba(0,255,209,0.12), transparent 55%),
        radial-gradient(900px 500px at 90% 10%, rgba(102,204,255,0.10), transparent 55%),
        radial-gradient(800px 500px at 50% 100%, rgba(255,59,129,0.06), transparent 60%),
        linear-gradient(180deg, #03040a, #070a14 35%, #040611);
      padding: 26px;
    }}

    .grid {{
      position: fixed;
      inset: 0;
      pointer-events: none;
      background-image:
        linear-gradient(to right, rgba(0,255,209,0.05) 1px, transparent 1px),
        linear-gradient(to bottom, rgba(0,255,209,0.04) 1px, transparent 1px);
      background-size: 44px 44px;
      mask-image: radial-gradient(circle at 50% 20%, rgba(0,0,0,1) 0%, rgba(0,0,0,0.75) 40%, rgba(0,0,0,0) 85%);
    }}

    .wrap {{ max-width: 1180px; margin: 0 auto; }}

    h1 {{
      margin: 0 0 10px 0;
      font-size: 34px;
      letter-spacing: 0.4px;
      text-transform: uppercase;
    }}

    h2 {{
      margin: 26px 0 10px 0;
      font-size: 18px;
      letter-spacing: 0.6px;
      text-transform: uppercase;
      color: rgba(230,241,255,0.9);
    }}

    .muted {{
      color: var(--muted);
      line-height: 1.45;
    }}

    .divider {{
      height: 1px;
      background: linear-gradient(90deg, transparent, var(--line), transparent);
      margin: 18px 0 6px 0;
    }}

    .cards {{
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(240px, 1fr));
      gap: 14px;
      margin: 18px 0 22px 0;
    }}

    .card {{
      background: linear-gradient(180deg, rgba(12,18,35,0.84), rgba(8,12,24,0.72));
      border: 1px solid var(--line);
      border-radius: var(--radius);
      box-shadow: var(--shadow);
      padding: 14px 16px;
      position: relative;
      overflow: hidden;
    }}

    .card::before {{
      content: "";
      position: absolute;
      inset: 0;
      background:
        radial-gradient(400px 120px at 10% 10%, rgba(0,255,209,0.12), transparent 60%),
        radial-gradient(380px 110px at 90% 20%, rgba(102,204,255,0.10), transparent 60%);
      opacity: 0.9;
      pointer-events: none;
    }}

    .card .label {{
      position: relative;
      color: var(--muted);
      font-size: 12px;
      letter-spacing: 0.6px;
      text-transform: uppercase;
    }}

    .card .value {{
      position: relative;
      font-size: 26px;
      margin-top: 8px;
      color: var(--accent);
      text-shadow: 0 0 12px rgba(0,255,209,0.18);
    }}

    table {{
      width: 100%;
      border-collapse: collapse;
      margin: 10px 0 18px 0;
      background: rgba(8,12,24,0.55);
      border: 1px solid var(--line2);
      border-radius: var(--radius);
      overflow: hidden;
      box-shadow: var(--shadow);
    }}

    th, td {{
      padding: 10px 12px;
      border-bottom: 1px solid rgba(102,204,255,0.10);
      font-size: 13px;
      vertical-align: top;
    }}

    th {{
      text-align: left;
      color: rgba(230,241,255,0.85);
      background: rgba(12,18,35,0.65);
      letter-spacing: 0.6px;
      text-transform: uppercase;
    }}

    tr:hover td {{ background: rgba(0,255,209,0.04); }}
    td {{ color: rgba(230,241,255,0.88); }}
    a {{ color: var(--accent2); text-decoration: none; }}
    a:hover {{ text-decoration: underline; }}
  </style>
</head>
<body>
  <div class="grid"></div>
  <div class="wrap">
    <h1>Aether IPC — Benchmark Dashboard</h1>
    <div class="muted">
      Host: <b>{host}</b> &bull; CPUs: <b>{cpus}</b> @ <b>{mhz:.0f} MHz</b><br/>
      Generated: <b>{now}</b>
    </div>

    <div class="divider"></div>

    <div class="cards">
      <div class="card">
        <div class="label">Best Ring Write</div>
        <div class="value">{format_time(best_ring_ns)}</div>
      </div>
      <div class="card">
        <div class="label">Best IPC Call</div>
        <div class="value">{format_time(best_call_ns)}</div>
      </div>
      <div class="card">
        <div class="label">Best Notify</div>
        <div class="value">{format_time(best_notify_ns)}</div>
      </div>
      <div class="card">
        <div class="label">Peak Throughput</div>
        <div class="value">{format_rate(best_ops, "op/s")}</div>
      </div>
    </div>

{tables}
    <h2>Raw Data</h2>
    <div class="muted">
      <a href="dev/bench/index.html">Historical trend charts</a> &bull;
      <a href="bench/benchmark_result.json">benchmark_result.json</a>
    </div>
  </div>
</body>
</html>"""

    print(html)


if __name__ == "__main__":
    main()
