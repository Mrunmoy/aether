window.BENCHMARK_DATA = {
  "lastUpdate": 1774525249040,
  "repoUrl": "https://github.com/Mrunmoy/aether",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "Mrunmoy@users.noreply.github.com",
            "name": "Mrunmoy Samal",
            "username": "Mrunmoy"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "12ca0e8b8b7a98e9ac2a530d6b63b048ee678313",
          "message": "Pass github-token to benchmark action for gh-pages push (#41)",
          "timestamp": "2026-03-26T21:51:59+11:00",
          "tree_id": "168eac3de4429d449595db3b2e6c6dedb44ace4b",
          "url": "https://github.com/Mrunmoy/aether/commit/12ca0e8b8b7a98e9ac2a530d6b63b048ee678313"
        },
        "date": 1774522422559,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 62504.09148936371,
            "unit": "ns/iter",
            "extra": "iterations: 11280\ncpu: 62464.268262411344 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 61677.347097230966,
            "unit": "ns/iter",
            "extra": "iterations: 11334\ncpu: 61645.53493912124 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 61893.832848452636,
            "unit": "ns/iter",
            "extra": "iterations: 11343\ncpu: 61858.70968879485 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 62672.90614364941,
            "unit": "ns/iter",
            "extra": "iterations: 11166\ncpu: 62642.73956654126 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 64209.64095964478,
            "unit": "ns/iter",
            "extra": "iterations: 10879\ncpu: 64181.61623310967 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2949.885723109144,
            "unit": "ns/iter",
            "extra": "iterations: 236382\ncpu: 2949.725156737826 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 2974.303309723942,
            "unit": "ns/iter",
            "extra": "iterations: 234793\ncpu: 2974.2527715902943 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3037.544757066708,
            "unit": "ns/iter",
            "extra": "iterations: 230701\ncpu: 3037.38050116818 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4305.126893191982,
            "unit": "ns/iter",
            "extra": "iterations: 162160\ncpu: 4305.061001480025 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7399.385525531126,
            "unit": "ns/iter",
            "extra": "iterations: 90518\ncpu: 7398.444132658694 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2951.4756664469032,
            "unit": "ns/iter",
            "extra": "iterations: 236628\ncpu: 2951.448230978578 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 2975.098652231641,
            "unit": "ns/iter",
            "extra": "iterations: 235352\ncpu: 2974.910300316123 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3040.3385232030682,
            "unit": "ns/iter",
            "extra": "iterations: 231027\ncpu: 3040.240084492286 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 4294.495767367816,
            "unit": "ns/iter",
            "extra": "iterations: 162901\ncpu: 4294.369709209886 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.3541154594740783,
            "unit": "ns/iter",
            "extra": "iterations: 515944772\ncpu: 1.3540845957055243 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 94.7572790414246,
            "unit": "us/iter",
            "extra": "iterations: 7386\ncpu: 94.74553154616822 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 18.58558306008027,
            "unit": "us/iter",
            "extra": "iterations: 37816\ncpu: 18.584711550666356 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 46.352691967965804,
            "unit": "us/iter",
            "extra": "iterations: 50946\ncpu: 13.921316668629542 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 47.06071589496884,
            "unit": "us/iter",
            "extra": "iterations: 49890\ncpu: 13.69716658649031 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 47.64287696312082,
            "unit": "us/iter",
            "extra": "iterations: 45336\ncpu: 13.911872331039316 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 57.093120957062645,
            "unit": "us/iter",
            "extra": "iterations: 44553\ncpu: 14.58823697618564 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 47.41270553458384,
            "unit": "us/iter",
            "extra": "iterations: 50067\ncpu: 13.961484650568256 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 489.47493333333824,
            "unit": "us/iter",
            "extra": "iterations: 1560\ncpu: 445.4675358974355 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 47.25498195248517,
            "unit": "us/iter",
            "extra": "iterations: 50090\ncpu: 14.26697987622282 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 94.18014861857272,
            "unit": "us/iter",
            "extra": "iterations: 26060\ncpu: 28.371257329240233 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 191.65176145488934,
            "unit": "us/iter",
            "extra": "iterations: 12702\ncpu: 58.070532357108924 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 389.24082865843286,
            "unit": "us/iter",
            "extra": "iterations: 6239\ncpu: 113.97326815194776 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.2639210704092925,
            "unit": "us/iter",
            "extra": "iterations: 553620\ncpu: 1.2638705538094692 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.2533168622952149,
            "unit": "us/iter",
            "extra": "iterations: 554367\ncpu: 1.2532594346344554 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.2999082736096228,
            "unit": "us/iter",
            "extra": "iterations: 543475\ncpu: 1.299626262477579 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 2.1411238287793233,
            "unit": "us/iter",
            "extra": "iterations: 325942\ncpu: 2.1410003160071387 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.2513096851327603,
            "unit": "us/iter",
            "extra": "iterations: 565413\ncpu: 1.2512181237431734 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 4.244668357573664,
            "unit": "us/iter",
            "extra": "iterations: 178011\ncpu: 4.2443562645005075 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 10.733920300060218,
            "unit": "us/iter",
            "extra": "iterations: 73852\ncpu: 9.736582909061378 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 30.15531003920932,
            "unit": "us/iter",
            "extra": "iterations: 24484\ncpu: 27.994867668681565 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 64.67633271415437,
            "unit": "us/iter",
            "extra": "iterations: 13997\ncpu: 59.48211688218925 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.2530018445469897,
            "unit": "us/iter",
            "extra": "iterations: 555692\ncpu: 1.2528121963245782 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "Mrunmoy@users.noreply.github.com",
            "name": "Mrunmoy Samal",
            "username": "Mrunmoy"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "a2561a1b7549c9d2983cd305cb59bbdb46819098",
          "message": "Add custom benchmark dashboard for GitHub Pages (#42)\n\nGenerate Ouroboros-style dark-theme HTML dashboard from Google Benchmark\nJSON output. Hero cards show best ring write, IPC call, notify latency and\npeak throughput. Tables grouped by Ring I/O, IPC Round-Trip, Notifications.\nCI workflow generates and deploys the page alongside the existing trend\ncharts.",
          "timestamp": "2026-03-26T22:11:56+11:00",
          "tree_id": "91bb41f844d128c94504a3a4a71718f6b9d87c39",
          "url": "https://github.com/Mrunmoy/aether/commit/a2561a1b7549c9d2983cd305cb59bbdb46819098"
        },
        "date": 1774523620355,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 71104.2311522036,
            "unit": "ns/iter",
            "extra": "iterations: 9842\ncpu: 71033.35582198741 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 70801.1311918345,
            "unit": "ns/iter",
            "extra": "iterations: 9993\ncpu: 70733.41178825177 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 71633.57537611383,
            "unit": "ns/iter",
            "extra": "iterations: 9771\ncpu: 71504.95844846994 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 71877.96318326233,
            "unit": "ns/iter",
            "extra": "iterations: 9751\ncpu: 71746.83211978262 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 73458.41360872406,
            "unit": "ns/iter",
            "extra": "iterations: 9538\ncpu: 73328.68316208848 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2949.885832968555,
            "unit": "ns/iter",
            "extra": "iterations: 237608\ncpu: 2949.423668395005 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 2979.5133669565903,
            "unit": "ns/iter",
            "extra": "iterations: 234758\ncpu: 2979.0667623680542 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3059.427492401277,
            "unit": "ns/iter",
            "extra": "iterations: 230631\ncpu: 3059.0809865109204 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 3930.5214735304057,
            "unit": "ns/iter",
            "extra": "iterations: 177940\ncpu: 3930.2019163763084 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7097.012376614282,
            "unit": "ns/iter",
            "extra": "iterations: 98573\ncpu: 7096.598713643687 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2946.792184006795,
            "unit": "ns/iter",
            "extra": "iterations: 237513\ncpu: 2946.5832522851374 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 2978.079531083017,
            "unit": "ns/iter",
            "extra": "iterations: 234839\ncpu: 2977.857638637538 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3029.68364937631,
            "unit": "ns/iter",
            "extra": "iterations: 230823\ncpu: 3029.5211828977144 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 3926.2605337946165,
            "unit": "ns/iter",
            "extra": "iterations: 178046\ncpu: 3926.1706637610473 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.3493603994439123,
            "unit": "ns/iter",
            "extra": "iterations: 517710932\ncpu: 1.349259661760435 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 94.72433784149429,
            "unit": "us/iter",
            "extra": "iterations: 7394\ncpu: 94.7208475791182 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 18.538552294584857,
            "unit": "us/iter",
            "extra": "iterations: 38264\ncpu: 18.537836321346393 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 45.385296585725115,
            "unit": "us/iter",
            "extra": "iterations: 48883\ncpu: 14.339226397725206 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 45.319454574219044,
            "unit": "us/iter",
            "extra": "iterations: 47407\ncpu: 14.36917109287663 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 45.64561312337034,
            "unit": "us/iter",
            "extra": "iterations: 49088\ncpu: 14.153057223761456 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 54.218747252516344,
            "unit": "us/iter",
            "extra": "iterations: 47589\ncpu: 14.943845174305027 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 45.36409337468678,
            "unit": "us/iter",
            "extra": "iterations: 45462\ncpu: 14.545569772557272 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 538.8216816901432,
            "unit": "us/iter",
            "extra": "iterations: 1420\ncpu: 491.09127323943636 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 45.639685679113505,
            "unit": "us/iter",
            "extra": "iterations: 46303\ncpu: 14.445575362287576 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 92.15997081712038,
            "unit": "us/iter",
            "extra": "iterations: 24672\ncpu: 28.62199051556419 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 185.27211917302097,
            "unit": "us/iter",
            "extra": "iterations: 12721\ncpu: 57.65485425674084 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 372.9471262996446,
            "unit": "us/iter",
            "extra": "iterations: 5867\ncpu: 117.54944077041065 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.6237682692552227,
            "unit": "us/iter",
            "extra": "iterations: 432571\ncpu: 1.6236381703812752 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.6276366010282985,
            "unit": "us/iter",
            "extra": "iterations: 431201\ncpu: 1.6275580576111852 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.3056525522664792,
            "unit": "us/iter",
            "extra": "iterations: 428286\ncpu: 1.3055042378223896 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 2.3477519308277586,
            "unit": "us/iter",
            "extra": "iterations: 290290\ncpu: 2.3474629921802306 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.6302224988472045,
            "unit": "us/iter",
            "extra": "iterations: 429382\ncpu: 1.6301046061548923 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 4.040139644479889,
            "unit": "us/iter",
            "extra": "iterations: 130288\ncpu: 4.039919017868118 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 16.987722445426126,
            "unit": "us/iter",
            "extra": "iterations: 44802\ncpu: 15.825158720592835 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 38.90392203135945,
            "unit": "us/iter",
            "extra": "iterations: 18687\ncpu: 37.353144699523796 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 80.49675629694981,
            "unit": "us/iter",
            "extra": "iterations: 9052\ncpu: 76.88607490057477 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.6192129317636825,
            "unit": "us/iter",
            "extra": "iterations: 432702\ncpu: 1.6190840301177214 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "Mrunmoy@users.noreply.github.com",
            "name": "Mrunmoy Samal",
            "username": "Mrunmoy"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "dce9be350ffa571faf9172d1e5b4baec8e825eda",
          "message": "Add transport comparison benchmarks and feature matrix (#43)\n\n* Add transport comparison benchmarks and feature matrix\n\n* Add CI and benchmark badges to README",
          "timestamp": "2026-03-26T22:38:28+11:00",
          "tree_id": "4971353790016c1ccf0373964d25497ba3160dec",
          "url": "https://github.com/Mrunmoy/aether/commit/dce9be350ffa571faf9172d1e5b4baec8e825eda"
        },
        "date": 1774525248282,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 3501.579414886621,
            "unit": "ns/iter",
            "extra": "iterations: 204439\ncpu: 3501.376117081378 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 3481.047913937543,
            "unit": "ns/iter",
            "extra": "iterations: 201528\ncpu: 3480.7304146322094 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 3499.3951879924193,
            "unit": "ns/iter",
            "extra": "iterations: 200540\ncpu: 3498.7794704298394 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 4112.821902661345,
            "unit": "ns/iter",
            "extra": "iterations: 170929\ncpu: 4112.5016351818595 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 73403.33711048267,
            "unit": "ns/iter",
            "extra": "iterations: 9531\ncpu: 73333.68911971465 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2940.3905450178136,
            "unit": "ns/iter",
            "extra": "iterations: 238139\ncpu: 2940.2284674076914 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 2963.566117561173,
            "unit": "ns/iter",
            "extra": "iterations: 235656\ncpu: 2963.3746732525397 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3015.282760345307,
            "unit": "ns/iter",
            "extra": "iterations: 231942\ncpu: 3015.066512317735 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4061.6298430371044,
            "unit": "ns/iter",
            "extra": "iterations: 174245\ncpu: 4061.3409566989067 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7110.2858076071425,
            "unit": "ns/iter",
            "extra": "iterations: 94910\ncpu: 7109.742714150242 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2929.2945878713344,
            "unit": "ns/iter",
            "extra": "iterations: 239185\ncpu: 2929.056337144892 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 2968.322564650261,
            "unit": "ns/iter",
            "extra": "iterations: 235962\ncpu: 2968.0587467473592 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3015.3459347085895,
            "unit": "ns/iter",
            "extra": "iterations: 231853\ncpu: 3015.128525401877 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 3914.1594925134104,
            "unit": "ns/iter",
            "extra": "iterations: 174980\ncpu: 3913.853674705686 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.350457867147337,
            "unit": "ns/iter",
            "extra": "iterations: 518219207\ncpu: 1.3503761025206458 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 95.46307598039148,
            "unit": "us/iter",
            "extra": "iterations: 7344\ncpu: 95.45519131263619 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 18.505880906575303,
            "unit": "us/iter",
            "extra": "iterations: 37945\ncpu: 18.503571247858734 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 44.8487651580684,
            "unit": "us/iter",
            "extra": "iterations: 50864\ncpu: 14.226788691412404 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 45.711230881151295,
            "unit": "us/iter",
            "extra": "iterations: 50173\ncpu: 14.695682458692918 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 45.930901865620946,
            "unit": "us/iter",
            "extra": "iterations: 51511\ncpu: 14.331990235095425 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 55.11873787921102,
            "unit": "us/iter",
            "extra": "iterations: 42984\ncpu: 15.704580867299512 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 45.711218916758455,
            "unit": "us/iter",
            "extra": "iterations: 46287\ncpu: 14.642953637090265 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 543.7798415770704,
            "unit": "us/iter",
            "extra": "iterations: 1395\ncpu: 493.7124594982074 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 45.97898860604958,
            "unit": "us/iter",
            "extra": "iterations: 44234\ncpu: 14.71330587331011 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 93.12538283072301,
            "unit": "us/iter",
            "extra": "iterations: 24008\ncpu: 28.782726716094547 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 187.09304138462892,
            "unit": "us/iter",
            "extra": "iterations: 11671\ncpu: 58.64138668494573 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 378.1581960431651,
            "unit": "us/iter",
            "extra": "iterations: 6116\ncpu: 118.81793034663187 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.6331305907727973,
            "unit": "us/iter",
            "extra": "iterations: 425612\ncpu: 1.632808830578088 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.6427472088863777,
            "unit": "us/iter",
            "extra": "iterations: 424920\ncpu: 1.64261943424645 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.6471439799102172,
            "unit": "us/iter",
            "extra": "iterations: 423698\ncpu: 1.64688865890327 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 1.1901955080000022,
            "unit": "us/iter",
            "extra": "iterations: 1000000\ncpu: 1.1900762730000025 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.6527873165767264,
            "unit": "us/iter",
            "extra": "iterations: 416591\ncpu: 1.6526897100513476 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 5.698625219288724,
            "unit": "us/iter",
            "extra": "iterations: 116855\ncpu: 5.698516469128391 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 17.23244692121426,
            "unit": "us/iter",
            "extra": "iterations: 41721\ncpu: 15.977066728985344 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 38.98153331178983,
            "unit": "us/iter",
            "extra": "iterations: 18567\ncpu: 37.476294716432456 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 81.43563836913718,
            "unit": "us/iter",
            "extra": "iterations: 8658\ncpu: 78.1315967890972 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.6704872462613518,
            "unit": "us/iter",
            "extra": "iterations: 426620\ncpu: 1.6701564014814023 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 45.59894259963023,
            "unit": "us/iter",
            "extra": "iterations: 48153\ncpu: 14.709194858056522 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 46.08291436954991,
            "unit": "us/iter",
            "extra": "iterations: 48394\ncpu: 14.978967289333351 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 54.959494185914096,
            "unit": "us/iter",
            "extra": "iterations: 43945\ncpu: 15.036934941404061 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 16.022982138585697,
            "unit": "us/iter",
            "extra": "iterations: 87787\ncpu: 8.258008030801827 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 16.463608422415174,
            "unit": "us/iter",
            "extra": "iterations: 80357\ncpu: 8.742654305163196 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 20.122810146275828,
            "unit": "us/iter",
            "extra": "iterations: 65561\ncpu: 11.755184866002635 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 24.775663970209013,
            "unit": "us/iter",
            "extra": "iterations: 71699\ncpu: 10.827344174953566 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 26.453987117900468,
            "unit": "us/iter",
            "extra": "iterations: 60161\ncpu: 12.327116853110846 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 34.64036776250701,
            "unit": "us/iter",
            "extra": "iterations: 45475\ncpu: 16.63640356239683 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 43.19871418034901,
            "unit": "us/iter",
            "extra": "iterations: 31184\ncpu: 21.826506926629005 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 43.37410611678724,
            "unit": "us/iter",
            "extra": "iterations: 37487\ncpu: 21.788678688612098 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 48.89776947712872,
            "unit": "us/iter",
            "extra": "iterations: 28726\ncpu: 24.885959305158973 us\nthreads: 1"
          }
        ]
      }
    ]
  }
}