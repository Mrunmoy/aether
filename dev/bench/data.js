window.BENCHMARK_DATA = {
  "lastUpdate": 1774643719368,
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
          "id": "f144c61cc3b3c1d77d8b074c3e60b1048b72d4d8",
          "message": "Merge pull request #44 from Mrunmoy/codex/backlog-1-sanitizers-clean\n\nAdd sanitizer CI matrix and fix sanitizer issues",
          "timestamp": "2026-03-26T22:47:11+11:00",
          "tree_id": "6ee075f6410823aedd50c901acfc7514cc07323d",
          "url": "https://github.com/Mrunmoy/aether/commit/f144c61cc3b3c1d77d8b074c3e60b1048b72d4d8"
        },
        "date": 1774525762988,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 3552.6660881945145,
            "unit": "ns/iter",
            "extra": "iterations: 192461\ncpu: 3552.450454897356 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 3425.5357623974683,
            "unit": "ns/iter",
            "extra": "iterations: 204880\ncpu: 3424.62581999219 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 3437.776460855918,
            "unit": "ns/iter",
            "extra": "iterations: 203083\ncpu: 3437.14216847299 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 3899.145588251494,
            "unit": "ns/iter",
            "extra": "iterations: 181402\ncpu: 3898.6207980066365 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 73271.51241060138,
            "unit": "ns/iter",
            "extra": "iterations: 9508\ncpu: 73193.961295751 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2950.399734289382,
            "unit": "ns/iter",
            "extra": "iterations: 237100\ncpu: 2950.188954027838 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 2978.816465045148,
            "unit": "ns/iter",
            "extra": "iterations: 235590\ncpu: 2978.36189991086 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3022.671351967958,
            "unit": "ns/iter",
            "extra": "iterations: 231433\ncpu: 3022.5795586627673 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4003.944657288727,
            "unit": "ns/iter",
            "extra": "iterations: 174097\ncpu: 4003.7162903438943 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7690.704954324775,
            "unit": "ns/iter",
            "extra": "iterations: 82211\ncpu: 7690.648100619137 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2939.7671265651334,
            "unit": "ns/iter",
            "extra": "iterations: 238241\ncpu: 2939.659382725892 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 3048.3645017201247,
            "unit": "ns/iter",
            "extra": "iterations: 235741\ncpu: 3048.140768894673 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3032.3566703227016,
            "unit": "ns/iter",
            "extra": "iterations: 231578\ncpu: 3032.1445819551077 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 3928.43975300589,
            "unit": "ns/iter",
            "extra": "iterations: 177818\ncpu: 3928.278785049888 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.3522015995112036,
            "unit": "ns/iter",
            "extra": "iterations: 518461257\ncpu: 1.3521173424921897 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 99.34642964538952,
            "unit": "us/iter",
            "extra": "iterations: 7050\ncpu: 99.33795375886513 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 18.472414591029008,
            "unit": "us/iter",
            "extra": "iterations: 37900\ncpu: 18.470365250659626 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 42.14024430761202,
            "unit": "us/iter",
            "extra": "iterations: 53756\ncpu: 12.957219008110746 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 43.36432687822188,
            "unit": "us/iter",
            "extra": "iterations: 51019\ncpu: 13.442948960191305 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 43.22986771849697,
            "unit": "us/iter",
            "extra": "iterations: 48170\ncpu: 13.46723780361219 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 52.14165913843578,
            "unit": "us/iter",
            "extra": "iterations: 47820\ncpu: 13.860527478042734 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 42.84350234817238,
            "unit": "us/iter",
            "extra": "iterations: 56001\ncpu: 12.91647974143318 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 534.592719943427,
            "unit": "us/iter",
            "extra": "iterations: 1414\ncpu: 489.15357991513514 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 42.80183276955989,
            "unit": "us/iter",
            "extra": "iterations: 54691\ncpu: 13.37988402113697 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 86.66307436354633,
            "unit": "us/iter",
            "extra": "iterations: 26357\ncpu: 26.50488409151268 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 175.04944837920863,
            "unit": "us/iter",
            "extra": "iterations: 12679\ncpu: 54.254052054578416 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 351.03613856950824,
            "unit": "us/iter",
            "extra": "iterations: 6697\ncpu: 107.08920188143973 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.6372093382612356,
            "unit": "us/iter",
            "extra": "iterations: 427767\ncpu: 1.6370739584867513 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.6325629732881641,
            "unit": "us/iter",
            "extra": "iterations: 426141\ncpu: 1.6324247537786778 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.64825259501645,
            "unit": "us/iter",
            "extra": "iterations: 422348\ncpu: 1.6481962102342103 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 2.386879976290294,
            "unit": "us/iter",
            "extra": "iterations: 300299\ncpu: 2.386746998824502 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.6846702609508073,
            "unit": "us/iter",
            "extra": "iterations: 415289\ncpu: 1.684560315828251 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 5.063682143950082,
            "unit": "us/iter",
            "extra": "iterations: 140507\ncpu: 5.062953141124638 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 16.858636138120467,
            "unit": "us/iter",
            "extra": "iterations: 45149\ncpu: 15.456890916742452 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 37.254247864112486,
            "unit": "us/iter",
            "extra": "iterations: 19781\ncpu: 35.544089075375084 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 78.45690386658255,
            "unit": "us/iter",
            "extra": "iterations: 9414\ncpu: 74.70364563416221 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.6435362926822137,
            "unit": "us/iter",
            "extra": "iterations: 415249\ncpu: 1.643466972828348 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 43.017351979137096,
            "unit": "us/iter",
            "extra": "iterations: 49466\ncpu: 13.103052379412084 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 43.01937192935843,
            "unit": "us/iter",
            "extra": "iterations: 52717\ncpu: 13.138348104027273 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 52.41641535530698,
            "unit": "us/iter",
            "extra": "iterations: 50393\ncpu: 14.266919631694874 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 18.112678063996523,
            "unit": "us/iter",
            "extra": "iterations: 67004\ncpu: 9.595508924840315 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 15.816413695461437,
            "unit": "us/iter",
            "extra": "iterations: 75222\ncpu: 8.28502133684294 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 19.7027416775695,
            "unit": "us/iter",
            "extra": "iterations: 68790\ncpu: 11.627787265590895 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 24.20614663302997,
            "unit": "us/iter",
            "extra": "iterations: 60648\ncpu: 10.666578617596608 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 23.690681543127813,
            "unit": "us/iter",
            "extra": "iterations: 66929\ncpu: 10.29579180923075 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 31.271499453456734,
            "unit": "us/iter",
            "extra": "iterations: 45742\ncpu: 14.307363779458658 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 38.18281170985861,
            "unit": "us/iter",
            "extra": "iterations: 36465\ncpu: 18.847705772658752 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 40.55150293274041,
            "unit": "us/iter",
            "extra": "iterations: 38701\ncpu: 20.028702617503452 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 44.59439216323312,
            "unit": "us/iter",
            "extra": "iterations: 30778\ncpu: 22.129544057443578 us\nthreads: 1"
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
          "id": "c0ee823cda5a2742bb1e88b165d3f285411cce07",
          "message": "Improve comparison benchmarks with framing and 64KB payload (#46)\n\nAdd length-prefixed framing to raw transport benchmarks so the\ncomparison is apples-to-apples. Add 64KB payload size where Aether\nshared-memory transport beats pipes and approaches UDS while\nproviding a complete typed RPC framework. Update dashboard text\nto explain what each transport includes.",
          "timestamp": "2026-03-26T22:57:05+11:00",
          "tree_id": "99d71b1b3ad97840678edfbc4e0a2e0fa110b9f9",
          "url": "https://github.com/Mrunmoy/aether/commit/c0ee823cda5a2742bb1e88b165d3f285411cce07"
        },
        "date": 1774526366538,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 71279.73691712365,
            "unit": "ns/iter",
            "extra": "iterations: 9822\ncpu: 71135.84432905722 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 71038.32504761266,
            "unit": "ns/iter",
            "extra": "iterations: 9977\ncpu: 70867.48291069461 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 71174.51663106041,
            "unit": "ns/iter",
            "extra": "iterations: 9831\ncpu: 71015.15095107314 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 71938.3632856156,
            "unit": "ns/iter",
            "extra": "iterations: 9849\ncpu: 71783.51954513148 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 72871.51674142163,
            "unit": "ns/iter",
            "extra": "iterations: 9587\ncpu: 72720.92458537604 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 3036.0827362577993,
            "unit": "ns/iter",
            "extra": "iterations: 230987\ncpu: 3035.377778836036 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 3089.466513484771,
            "unit": "ns/iter",
            "extra": "iterations: 227181\ncpu: 3088.6377337893605 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3146.3199694437358,
            "unit": "ns/iter",
            "extra": "iterations: 222540\ncpu: 3146.0869821155748 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4046.348047076539,
            "unit": "ns/iter",
            "extra": "iterations: 172741\ncpu: 4046.130160182013 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7374.339314947367,
            "unit": "ns/iter",
            "extra": "iterations: 95292\ncpu: 7373.891554380213 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 3068.8615210537346,
            "unit": "ns/iter",
            "extra": "iterations: 228013\ncpu: 3068.5531351282675 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 3117.8334924599935,
            "unit": "ns/iter",
            "extra": "iterations: 224140\ncpu: 3117.5904747033123 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3172.282819506734,
            "unit": "ns/iter",
            "extra": "iterations: 220989\ncpu: 3172.022584834543 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 4050.2170272963453,
            "unit": "ns/iter",
            "extra": "iterations: 172476\ncpu: 4049.835519144693 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.557718466724333,
            "unit": "ns/iter",
            "extra": "iterations: 449244011\ncpu: 1.5576421229130184 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 97.2718046614804,
            "unit": "us/iter",
            "extra": "iterations: 7208\ncpu: 97.27106562153166 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.379390602949616,
            "unit": "us/iter",
            "extra": "iterations: 36139\ncpu: 19.37858391211715 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 45.66491243295786,
            "unit": "us/iter",
            "extra": "iterations: 47358\ncpu: 14.294575615524366 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 45.797635805559906,
            "unit": "us/iter",
            "extra": "iterations: 45937\ncpu: 14.389399046520234 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 45.80243802568445,
            "unit": "us/iter",
            "extra": "iterations: 48746\ncpu: 15.075784843884636 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 55.06940037954828,
            "unit": "us/iter",
            "extra": "iterations: 46371\ncpu: 15.212821892993436 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 46.32225380211972,
            "unit": "us/iter",
            "extra": "iterations: 52931\ncpu: 14.482212975383066 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 541.771037562011,
            "unit": "us/iter",
            "extra": "iterations: 1411\ncpu: 493.3620581148122 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 46.16636910740596,
            "unit": "us/iter",
            "extra": "iterations: 47390\ncpu: 14.901524709854383 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 93.68912221402915,
            "unit": "us/iter",
            "extra": "iterations: 24408\ncpu: 29.67324696820725 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 188.18009288674068,
            "unit": "us/iter",
            "extra": "iterations: 11584\ncpu: 60.08327468922646 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 375.374150803246,
            "unit": "us/iter",
            "extra": "iterations: 5789\ncpu: 119.09924546553805 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.6454500345893242,
            "unit": "us/iter",
            "extra": "iterations: 417759\ncpu: 1.6453848462869671 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.641129684447549,
            "unit": "us/iter",
            "extra": "iterations: 419233\ncpu: 1.6408091944097967 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.6411478920821794,
            "unit": "us/iter",
            "extra": "iterations: 426250\ncpu: 1.6409620293255138 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 0.663084679163294,
            "unit": "us/iter",
            "extra": "iterations: 2927190\ncpu: 0.6630368517246907 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.6304481765184105,
            "unit": "us/iter",
            "extra": "iterations: 428329\ncpu: 1.6303036194140492 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 5.2970634694700305,
            "unit": "us/iter",
            "extra": "iterations: 116623\ncpu: 5.296881652847212 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 17.744329759781966,
            "unit": "us/iter",
            "extra": "iterations: 42628\ncpu: 16.441607206530957 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 39.066248369209916,
            "unit": "us/iter",
            "extra": "iterations: 18396\ncpu: 37.38903565992635 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 81.30300402918668,
            "unit": "us/iter",
            "extra": "iterations: 9183\ncpu: 77.24063377981011 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.6395147848088414,
            "unit": "us/iter",
            "extra": "iterations: 420533\ncpu: 1.6394224638732315 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 46.40068200757504,
            "unit": "us/iter",
            "extra": "iterations: 47520\ncpu: 14.435000210437687 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 46.3452312662581,
            "unit": "us/iter",
            "extra": "iterations: 47668\ncpu: 14.530365213560433 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 55.72074890315283,
            "unit": "us/iter",
            "extra": "iterations: 46269\ncpu: 14.935013010871248 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 81.9599468255958,
            "unit": "us/iter",
            "extra": "iterations: 39850\ncpu: 17.003790740275964 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 18.16670962497889,
            "unit": "us/iter",
            "extra": "iterations: 53517\ncpu: 10.98901700394268 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 18.225523508327626,
            "unit": "us/iter",
            "extra": "iterations: 56916\ncpu: 11.412964807786926 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 21.23171624187541,
            "unit": "us/iter",
            "extra": "iterations: 44767\ncpu: 13.82101253155223 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 38.02825194268952,
            "unit": "us/iter",
            "extra": "iterations: 24708\ncpu: 26.04258802816908 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 26.958302325581922,
            "unit": "us/iter",
            "extra": "iterations: 58652\ncpu: 12.700332367182696 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 29.551264877731704,
            "unit": "us/iter",
            "extra": "iterations: 50831\ncpu: 14.68559052546669 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 35.55052835987017,
            "unit": "us/iter",
            "extra": "iterations: 37747\ncpu: 21.398968792221883 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 44.10853578491296,
            "unit": "us/iter",
            "extra": "iterations: 21238\ncpu: 32.294957858555684 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 52.136434712543775,
            "unit": "us/iter",
            "extra": "iterations: 22960\ncpu: 32.11437900696863 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 51.63120474291716,
            "unit": "us/iter",
            "extra": "iterations: 23825\ncpu: 31.919786023084942 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 52.48092014043276,
            "unit": "us/iter",
            "extra": "iterations: 22502\ncpu: 32.82519776019921 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 67.63009074608314,
            "unit": "us/iter",
            "extra": "iterations: 12827\ncpu: 48.83811506977472 us\nthreads: 1"
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
          "id": "bc4d0084829765671248e172a013003329993a06",
          "message": "Add CI runner caveat note to benchmark dashboard (#47)",
          "timestamp": "2026-03-26T23:05:11+11:00",
          "tree_id": "b47350ec811ce24741dcc51608bc02f78951c0c1",
          "url": "https://github.com/Mrunmoy/aether/commit/bc4d0084829765671248e172a013003329993a06"
        },
        "date": 1774526843616,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 72781.87788745352,
            "unit": "ns/iter",
            "extra": "iterations: 9827\ncpu: 72731.9127912893 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 72295.3445619806,
            "unit": "ns/iter",
            "extra": "iterations: 9737\ncpu: 72247.73153948857 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 72024.34534442013,
            "unit": "ns/iter",
            "extra": "iterations: 9741\ncpu: 71978.94507750742 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 74249.19288916768,
            "unit": "ns/iter",
            "extra": "iterations: 9591\ncpu: 74193.61755812741 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 74202.4645795491,
            "unit": "ns/iter",
            "extra": "iterations: 9359\ncpu: 74161.94796452613 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2988.4504223056147,
            "unit": "ns/iter",
            "extra": "iterations: 234309\ncpu: 2988.2822554831455 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 3025.051100662913,
            "unit": "ns/iter",
            "extra": "iterations: 231406\ncpu: 3024.8532924816122 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3089.61366393082,
            "unit": "ns/iter",
            "extra": "iterations: 225894\ncpu: 3089.147414273954 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 3915.9835605023673,
            "unit": "ns/iter",
            "extra": "iterations: 177621\ncpu: 3915.5600689107728 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7139.464154599233,
            "unit": "ns/iter",
            "extra": "iterations: 98060\ncpu: 7138.881960024461 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2988.666049403864,
            "unit": "ns/iter",
            "extra": "iterations: 233828\ncpu: 2988.476260328102 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 3025.642260616323,
            "unit": "ns/iter",
            "extra": "iterations: 231459\ncpu: 3025.489689318622 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3092.268621351557,
            "unit": "ns/iter",
            "extra": "iterations: 226635\ncpu: 3091.8786948176626 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 3915.699831101879,
            "unit": "ns/iter",
            "extra": "iterations: 178806\ncpu: 3915.3051072111675 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.4122088095318046,
            "unit": "ns/iter",
            "extra": "iterations: 497614889\ncpu: 1.412099618667158 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 95.82385008770628,
            "unit": "us/iter",
            "extra": "iterations: 7411\ncpu: 95.81434651194158 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.8163691005351,
            "unit": "us/iter",
            "extra": "iterations: 35321\ncpu: 19.815244698621232 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 40.9296282710646,
            "unit": "us/iter",
            "extra": "iterations: 57359\ncpu: 11.93236552241151 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 41.56510910974872,
            "unit": "us/iter",
            "extra": "iterations: 57905\ncpu: 12.193870978326572 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 42.04086959964937,
            "unit": "us/iter",
            "extra": "iterations: 59348\ncpu: 12.344427461751042 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 46.63346814292335,
            "unit": "us/iter",
            "extra": "iterations: 51888\ncpu: 13.792536174067198 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 41.713793118388274,
            "unit": "us/iter",
            "extra": "iterations: 55394\ncpu: 12.074533198541332 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 556.7874480994108,
            "unit": "us/iter",
            "extra": "iterations: 1368\ncpu: 513.652771929825 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 41.822818945599565,
            "unit": "us/iter",
            "extra": "iterations: 57132\ncpu: 12.402020111321166 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 83.25418508190974,
            "unit": "us/iter",
            "extra": "iterations: 28690\ncpu: 24.89222126176372 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 167.92949753331865,
            "unit": "us/iter",
            "extra": "iterations: 13581\ncpu: 49.10457639349096 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 337.08716628734,
            "unit": "us/iter",
            "extra": "iterations: 7030\ncpu: 99.90609914651493 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.2993529237791612,
            "unit": "us/iter",
            "extra": "iterations: 537609\ncpu: 1.2992456153077854 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.3143979087145246,
            "unit": "us/iter",
            "extra": "iterations: 513751\ncpu: 1.3143341424152934 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.4610279304134999,
            "unit": "us/iter",
            "extra": "iterations: 475897\ncpu: 1.4609756144712007 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 4.146471978948957,
            "unit": "us/iter",
            "extra": "iterations: 173673\ncpu: 4.145905851801945 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.323867453518356,
            "unit": "us/iter",
            "extra": "iterations: 525046\ncpu: 1.3238639414451288 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 4.478510124160628,
            "unit": "us/iter",
            "extra": "iterations: 155766\ncpu: 4.477884737362466 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 13.502866057674932,
            "unit": "us/iter",
            "extra": "iterations: 55830\ncpu: 12.457427404621182 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 32.5605458418466,
            "unit": "us/iter",
            "extra": "iterations: 23003\ncpu: 30.70867865061081 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 67.71517740749786,
            "unit": "us/iter",
            "extra": "iterations: 12243\ncpu: 63.440254921179495 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.3105649418742245,
            "unit": "us/iter",
            "extra": "iterations: 532122\ncpu: 1.3104819402317467 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 41.448105379426565,
            "unit": "us/iter",
            "extra": "iterations: 59300\ncpu: 12.073225750421658 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 41.46728365367971,
            "unit": "us/iter",
            "extra": "iterations: 57750\ncpu: 12.15626791342 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 46.22385275827101,
            "unit": "us/iter",
            "extra": "iterations: 52261\ncpu: 13.55563659325297 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 61.33398640816009,
            "unit": "us/iter",
            "extra": "iterations: 39509\ncpu: 17.848506593434276 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 18.79524367924214,
            "unit": "us/iter",
            "extra": "iterations: 65894\ncpu: 11.222689926245213 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 18.246145742659273,
            "unit": "us/iter",
            "extra": "iterations: 65252\ncpu: 11.078786749831442 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 19.92602246508793,
            "unit": "us/iter",
            "extra": "iterations: 44469\ncpu: 12.119847579212538 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 34.31380523812959,
            "unit": "us/iter",
            "extra": "iterations: 28178\ncpu: 23.544204095393507 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 24.518364149401076,
            "unit": "us/iter",
            "extra": "iterations: 63614\ncpu: 11.223160735058297 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 24.460128331521908,
            "unit": "us/iter",
            "extra": "iterations: 65323\ncpu: 11.17592183457583 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 28.01758852182084,
            "unit": "us/iter",
            "extra": "iterations: 50914\ncpu: 15.072516518050021 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 39.64725183202974,
            "unit": "us/iter",
            "extra": "iterations: 24290\ncpu: 29.245553272951955 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 43.93322854414914,
            "unit": "us/iter",
            "extra": "iterations: 28279\ncpu: 27.141954312387245 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 31.5224024171099,
            "unit": "us/iter",
            "extra": "iterations: 28381\ncpu: 23.950197420809655 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 45.070859300782885,
            "unit": "us/iter",
            "extra": "iterations: 23369\ncpu: 29.297311010312786 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 70.6763278166773,
            "unit": "us/iter",
            "extra": "iterations: 14139\ncpu: 50.31016889454716 us\nthreads: 1"
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
          "id": "af91c16a59b2d959ff5ba207e9cae852f0a8e601",
          "message": "Merge pull request #45 from Mrunmoy/codex/postmerge-sanitizer-review-fixes\n\nFix post-merge sanitizer review issues",
          "timestamp": "2026-03-26T23:11:21+11:00",
          "tree_id": "e8355b0f88083baaa3f9d79b114612137d6f501c",
          "url": "https://github.com/Mrunmoy/aether/commit/af91c16a59b2d959ff5ba207e9cae852f0a8e601"
        },
        "date": 1774527214640,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 71363.50486014712,
            "unit": "ns/iter",
            "extra": "iterations: 10082\ncpu: 71295.59779805594 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 69316.69763042548,
            "unit": "ns/iter",
            "extra": "iterations: 10044\ncpu: 69251.44563918759 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 69389.25264406385,
            "unit": "ns/iter",
            "extra": "iterations: 10117\ncpu: 69323.03439754869 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 70464.5638436152,
            "unit": "ns/iter",
            "extra": "iterations: 10001\ncpu: 70386.23567643233 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 72157.67447621013,
            "unit": "ns/iter",
            "extra": "iterations: 9689\ncpu: 72093.63597894517 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2977.3802812068466,
            "unit": "ns/iter",
            "extra": "iterations: 233138\ncpu: 2977.1335560912407 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 3102.439914931431,
            "unit": "ns/iter",
            "extra": "iterations: 231813\ncpu: 3102.3363357533867 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3104.7368042290213,
            "unit": "ns/iter",
            "extra": "iterations: 225110\ncpu: 3104.4736795344515 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 3995.1917066413284,
            "unit": "ns/iter",
            "extra": "iterations: 174694\ncpu: 3994.7817841482884 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7200.533247790748,
            "unit": "ns/iter",
            "extra": "iterations: 96638\ncpu: 7199.802644922281 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 3028.4236766321214,
            "unit": "ns/iter",
            "extra": "iterations: 231266\ncpu: 3027.9259251251847 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 3062.3805131351155,
            "unit": "ns/iter",
            "extra": "iterations: 228205\ncpu: 3061.940794461125 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3105.92747281061,
            "unit": "ns/iter",
            "extra": "iterations: 225088\ncpu: 3105.656725369636 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 3994.146438665063,
            "unit": "ns/iter",
            "extra": "iterations: 175145\ncpu: 3993.4992663221815 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.5583150621858255,
            "unit": "ns/iter",
            "extra": "iterations: 449008927\ncpu: 1.5582039485820727 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 97.20523189415036,
            "unit": "us/iter",
            "extra": "iterations: 7180\ncpu: 97.19601420612815 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.267561994163792,
            "unit": "us/iter",
            "extra": "iterations: 36326\ncpu: 19.266801629686725 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 42.63334682223024,
            "unit": "us/iter",
            "extra": "iterations: 52883\ncpu: 12.873112909630727 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 43.40775992671733,
            "unit": "us/iter",
            "extra": "iterations: 55129\ncpu: 13.080868200039863 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 43.31654648671807,
            "unit": "us/iter",
            "extra": "iterations: 51348\ncpu: 13.355263048220014 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 51.05559079779842,
            "unit": "us/iter",
            "extra": "iterations: 51053\ncpu: 13.410672066284041 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 42.858145573313806,
            "unit": "us/iter",
            "extra": "iterations: 53121\ncpu: 12.742438621260911 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 530.4656344683832,
            "unit": "us/iter",
            "extra": "iterations: 1439\ncpu: 482.3167199444053 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 43.30200766896567,
            "unit": "us/iter",
            "extra": "iterations: 54375\ncpu: 13.195711816091972 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 86.73739459850677,
            "unit": "us/iter",
            "extra": "iterations: 26252\ncpu: 26.162783140332113 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 176.6926263518662,
            "unit": "us/iter",
            "extra": "iterations: 13130\ncpu: 53.25087022086823 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 354.1969571865447,
            "unit": "us/iter",
            "extra": "iterations: 6540\ncpu: 108.81142186544344 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.632101163302686,
            "unit": "us/iter",
            "extra": "iterations: 431530\ncpu: 1.6320121938219836 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.6010192622168613,
            "unit": "us/iter",
            "extra": "iterations: 434789\ncpu: 1.6010039651417127 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.6202529226129365,
            "unit": "us/iter",
            "extra": "iterations: 436339\ncpu: 1.6201071185477351 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 0.10731044255371226,
            "unit": "us/iter",
            "extra": "iterations: 5554580\ncpu: 0.10728172589106638 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.6156122614044535,
            "unit": "us/iter",
            "extra": "iterations: 429021\ncpu: 1.6152841632460861 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 5.463467423789643,
            "unit": "us/iter",
            "extra": "iterations: 117110\ncpu: 5.461471949449233 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 17.841992611499364,
            "unit": "us/iter",
            "extra": "iterations: 44664\ncpu: 15.761782509403504 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 36.654510040160126,
            "unit": "us/iter",
            "extra": "iterations: 19920\ncpu: 35.31019974899593 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 75.8222578589641,
            "unit": "us/iter",
            "extra": "iterations: 9416\ncpu: 72.173341015293 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.6246511907221195,
            "unit": "us/iter",
            "extra": "iterations: 416302\ncpu: 1.6244877156487358 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 43.01754864819924,
            "unit": "us/iter",
            "extra": "iterations: 54113\ncpu: 12.770043797239095 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 43.27069555950979,
            "unit": "us/iter",
            "extra": "iterations: 51143\ncpu: 12.640339147097329 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 51.80682120620922,
            "unit": "us/iter",
            "extra": "iterations: 48184\ncpu: 14.170389901212065 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 77.4614046420847,
            "unit": "us/iter",
            "extra": "iterations: 42524\ncpu: 15.899269588938015 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 18.198051716401064,
            "unit": "us/iter",
            "extra": "iterations: 62398\ncpu: 10.962930718933354 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 17.618976940827817,
            "unit": "us/iter",
            "extra": "iterations: 59369\ncpu: 10.806153817648903 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 21.707339090340398,
            "unit": "us/iter",
            "extra": "iterations: 51184\ncpu: 13.870691153485563 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 34.95628090274231,
            "unit": "us/iter",
            "extra": "iterations: 29067\ncpu: 24.39729820758927 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 27.401127038626488,
            "unit": "us/iter",
            "extra": "iterations: 60580\ncpu: 13.19554749092114 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 26.950699100386636,
            "unit": "us/iter",
            "extra": "iterations: 61582\ncpu: 12.90952138611933 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 31.093055126901145,
            "unit": "us/iter",
            "extra": "iterations: 40307\ncpu: 17.310539434837683 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 43.9466093458746,
            "unit": "us/iter",
            "extra": "iterations: 23604\ncpu: 31.470535756651365 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 48.72182801358146,
            "unit": "us/iter",
            "extra": "iterations: 23560\ncpu: 29.485674660441212 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 45.885529651946634,
            "unit": "us/iter",
            "extra": "iterations: 25226\ncpu: 27.905188258146236 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 47.06537229713545,
            "unit": "us/iter",
            "extra": "iterations: 25066\ncpu: 29.52866675975416 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 62.12033928571461,
            "unit": "us/iter",
            "extra": "iterations: 16464\ncpu: 45.86541533041796 us\nthreads: 1"
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
          "id": "37e45d9194713bec99bbedb80cc16ada19435e86",
          "message": "Add ipcgen --backend python emitter scaffold (#36)\n\n* Add ipcgen --backend python emitter scaffold\n\nImplement a third ipcgen backend that generates typed Python client\nmodules from IDL files. The emitter produces a single .py file per\nservice with enum classes, struct classes (with pack/unpack), a typed\nclient class with RPC methods, and notification dispatch.\n\n- New python_emitter.py with emit_python_client() entry point\n- Wire --backend python into __main__.py CLI\n- 39 tests covering enums, structs, methods, notifications, strings,\n  arrays, all scalar types, and CLI integration\n\n* Fix blocking review issues in Python emitter\n\n- Wire _dispatch_notification to AetherClient via set_notification_handler\n  in generated __init__, so notifications actually flow from the transport\n- Add service_id != SERVICE_ID guard in _dispatch_notification to prevent\n  cross-service notification collisions\n- Change bool wire format from \"?\" to \"B\" (uint8) for C++ compatibility,\n  wrapping unpack results with bool() for correct Python types\n\n* Fix Copilot review issues in Python emitter\n\n- Add array length validation in struct field pack (raises ValueError\n  on size mismatch)\n- Add array length validation in method/notification param pack\n- Return correctly-sized zero-filled defaults for array out params\n- Use bytearray for O(n) pack performance instead of bytes concatenation\n- Remove unused imports (TextIO, TYPE_MAP)\n\n* Add documentation comments for LE wire format and enum design choices\n\nAddress Copilot review comments: clarify that LE format strings are\nintentional (matching aether wire protocol), explain enum-as-int-constant\ndesign, and annotate test assertions accordingly.\n\n* Fix misleading comment in _py_type_hint and add __all__ export\n\n* Guard generated Python identifiers against keyword collisions\n\nIDL identifiers like 'class', 'import', or 'def' would produce invalid\nPython code. Add _safe_name() using keyword.iskeyword() to append an\nunderscore when a generated name collides with a Python keyword. Applied\nto method names, param names, struct field names, and enum value names.\n\nAlso fix misleading comment: shared-memory IPC uses native byte order\n(not LE wire format), which is correct for x86-64 but would need\nadjustment on big-endian hosts.\n\n* Fix internal variable collisions and sanitize all type names\n\n- Prefix method internal locals with underscore (_rc, _resp, _req,\n  _offset, _timeout_ms) so IDL params named rc/resp/offset/etc.\n  don't shadow them\n- Prefix notification dispatch internals (_service_id, _message_id,\n  _payload, _offset) to prevent param name collisions\n- Add _safe_param() helper that avoids both Python keywords and\n  reserved internal locals\n- Apply _safe_name() to enum class names, struct class names, and\n  service client class names\n- Add TestSafeName and TestKeywordCollisionEndToEnd test classes\n  covering keyword mangling and reserved-name collision scenarios\n- Update existing test assertions to match new prefixed variable names\n\n* Fix review findings: string truncation, empty notification dispatch\n\n- Docstring: use \"rc\" instead of \"status\" for the IPC return code\n  so out-params named \"status\" are unambiguous\n- Guard _dispatch_notification: skip generating the method entirely\n  when the IDL service has no notifications block\n- UTF-8 string packing already encodes first then truncates to byte\n  length (encode→slice); verified correct, added no code change\n- Added tests: TestDocstringAmbiguity, TestNoNotificationDispatch",
          "timestamp": "2026-03-26T23:38:17+11:00",
          "tree_id": "ea50826de9617674f939f0de7321b84e27994a8d",
          "url": "https://github.com/Mrunmoy/aether/commit/37e45d9194713bec99bbedb80cc16ada19435e86"
        },
        "date": 1774528843165,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 71671.24152326591,
            "unit": "ns/iter",
            "extra": "iterations: 9821\ncpu: 71605.960085531 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 71234.35035086032,
            "unit": "ns/iter",
            "extra": "iterations: 9833\ncpu: 71160.30143394691 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 71270.41787709547,
            "unit": "ns/iter",
            "extra": "iterations: 9845\ncpu: 71185.3727780599 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 72273.43281427411,
            "unit": "ns/iter",
            "extra": "iterations: 9697\ncpu: 72204.5513045272 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 73443.70633710976,
            "unit": "ns/iter",
            "extra": "iterations: 9184\ncpu: 73381.74640679441 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2950.046979185629,
            "unit": "ns/iter",
            "extra": "iterations: 236807\ncpu: 2949.9700853437625 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 2985.9020984395606,
            "unit": "ns/iter",
            "extra": "iterations: 234174\ncpu: 2985.75491728373 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3050.4244942981013,
            "unit": "ns/iter",
            "extra": "iterations: 230274\ncpu: 3050.3141735497725 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 3901.8227072038867,
            "unit": "ns/iter",
            "extra": "iterations: 178930\ncpu: 3901.713770748335 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7785.613645616938,
            "unit": "ns/iter",
            "extra": "iterations: 98244\ncpu: 7785.444495338147 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2983.424004718364,
            "unit": "ns/iter",
            "extra": "iterations: 237370\ncpu: 2983.163487382572 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 3025.9202821199483,
            "unit": "ns/iter",
            "extra": "iterations: 231391\ncpu: 3025.826536036407 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3048.889023024684,
            "unit": "ns/iter",
            "extra": "iterations: 228624\ncpu: 3048.7203968087315 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 3914.055890926092,
            "unit": "ns/iter",
            "extra": "iterations: 178741\ncpu: 3913.7472991647173 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.557771903923943,
            "unit": "ns/iter",
            "extra": "iterations: 448686684\ncpu: 1.5577247128644454 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 97.21038569444495,
            "unit": "us/iter",
            "extra": "iterations: 7200\ncpu: 97.20790527777783 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.251189860534275,
            "unit": "us/iter",
            "extra": "iterations: 36353\ncpu: 19.24969710890434 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 45.360561871278705,
            "unit": "us/iter",
            "extra": "iterations: 49207\ncpu: 14.100680756802896 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 45.86011981800798,
            "unit": "us/iter",
            "extra": "iterations: 49233\ncpu: 14.051166900249834 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 46.2242454141799,
            "unit": "us/iter",
            "extra": "iterations: 54952\ncpu: 14.63670430557578 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 56.6878426012765,
            "unit": "us/iter",
            "extra": "iterations: 43717\ncpu: 16.02559873733331 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 45.61280678370424,
            "unit": "us/iter",
            "extra": "iterations: 47791\ncpu: 14.337197024544366 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 547.4492215868624,
            "unit": "us/iter",
            "extra": "iterations: 1399\ncpu: 497.61997998570365 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 46.44991651564358,
            "unit": "us/iter",
            "extra": "iterations: 50213\ncpu: 14.967627845378725 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 92.23855679178408,
            "unit": "us/iter",
            "extra": "iterations: 24537\ncpu: 29.037746097730018 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 187.1075159322026,
            "unit": "us/iter",
            "extra": "iterations: 11800\ncpu: 58.5387333898304 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 378.84240621095665,
            "unit": "us/iter",
            "extra": "iterations: 5603\ncpu: 119.26021060146354 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.6554311538807052,
            "unit": "us/iter",
            "extra": "iterations: 422522\ncpu: 1.655257655222687 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.6774547351170022,
            "unit": "us/iter",
            "extra": "iterations: 424678\ncpu: 1.6772476040670774 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.6524418072141351,
            "unit": "us/iter",
            "extra": "iterations: 424056\ncpu: 1.652315425321185 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 0.31798108696597016,
            "unit": "us/iter",
            "extra": "iterations: 11734130\ncpu: 0.31794691101939393 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.6633680997348748,
            "unit": "us/iter",
            "extra": "iterations: 397213\ncpu: 1.6632974147371808 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 5.423379306752758,
            "unit": "us/iter",
            "extra": "iterations: 115197\ncpu: 5.422897419203609 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 18.65772623152108,
            "unit": "us/iter",
            "extra": "iterations: 42346\ncpu: 17.11689921125969 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 39.3695265097359,
            "unit": "us/iter",
            "extra": "iterations: 18182\ncpu: 37.88918705312947 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 81.76906857407663,
            "unit": "us/iter",
            "extra": "iterations: 8633\ncpu: 78.14162434842973 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.6528307893548484,
            "unit": "us/iter",
            "extra": "iterations: 427349\ncpu: 1.6527104825330203 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 46.192065902633985,
            "unit": "us/iter",
            "extra": "iterations: 51743\ncpu: 14.619719131090157 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 46.622593717836956,
            "unit": "us/iter",
            "extra": "iterations: 50524\ncpu: 14.960237134826972 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 56.11012524324572,
            "unit": "us/iter",
            "extra": "iterations: 44194\ncpu: 16.430314024528112 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 85.36069974791764,
            "unit": "us/iter",
            "extra": "iterations: 36496\ncpu: 19.114003589434503 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 19.322744311222397,
            "unit": "us/iter",
            "extra": "iterations: 57965\ncpu: 11.91687457948752 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 17.06819455934808,
            "unit": "us/iter",
            "extra": "iterations: 63338\ncpu: 10.696955476964858 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 21.416348306841336,
            "unit": "us/iter",
            "extra": "iterations: 43410\ncpu: 13.843313913844801 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 45.18212511819822,
            "unit": "us/iter",
            "extra": "iterations: 23266\ncpu: 31.132479025187113 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 28.95218190693101,
            "unit": "us/iter",
            "extra": "iterations: 49168\ncpu: 14.528986780019542 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 29.80008544422701,
            "unit": "us/iter",
            "extra": "iterations: 53216\ncpu: 14.970473973992744 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 33.12178888499494,
            "unit": "us/iter",
            "extra": "iterations: 34242\ncpu: 19.213707260089798 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 44.32274111698308,
            "unit": "us/iter",
            "extra": "iterations: 21952\ncpu: 32.660687682215546 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 52.17654061814837,
            "unit": "us/iter",
            "extra": "iterations: 19154\ncpu: 32.16080876057226 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 52.49534719046047,
            "unit": "us/iter",
            "extra": "iterations: 24488\ncpu: 32.488000694217725 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 54.786030411528465,
            "unit": "us/iter",
            "extra": "iterations: 22623\ncpu: 34.390301595721375 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 72.71949619999987,
            "unit": "us/iter",
            "extra": "iterations: 10000\ncpu: 52.70582870000027 us\nthreads: 1"
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
          "id": "9d241d46cdd08b2778701eae2a95dee898ae11bb",
          "message": "Add Python transport layer for aether IPC (#37)\n\n* Add Python transport layer for aether IPC clients\n\nImplements the runtime library that generated Python clients use to\ncommunicate with C++ aether services over UDS + shared memory:\n\n- constants.py: protocol constants matching Types.h and Platform.h\n- ringbuffer.py: SPSC ring buffer reader/writer matching ouroboros layout\n- framing.py: FrameHeader pack/unpack and frame I/O over rings\n- transport.py: UDS connection, memfd_create, SCM_RIGHTS handshake\n- __init__.py: high-level AetherClient with call() and notifications\n\nIncludes 36 tests covering layout verification, round-trip framing,\nring wraparound, and full handshake integration with a mock server.\n\n* Fix blocking review issues: SPSC single-consumer, atomic writes, ARM warning\n\n- Restructure AetherClient so rx_ring is only read by a single dedicated\n  receiver thread, fixing the SPSC single-consumer violation. call() now\n  registers a pending slot and waits on a condition variable, matching the\n  C++ ClientBase pattern.\n\n- Combine header + payload into a single ring.write() in write_frame()\n  so head is advanced only once after all data is written, preventing\n  the consumer from seeing partial frames.\n\n- Add runtime platform check in ringbuffer.py that warns on non-x86\n  architectures where aligned 4-byte loads/stores are not guaranteed\n  atomic. Document the x86-64 assumption in the module docstring.\n\n* Fix Copilot review comments on Python transport\n\n- Validate service name fits in 107-byte abstract namespace limit (ValueError)\n- Use setsockopt(SO_SNDTIMEO) instead of settimeout() to match C++ behaviour\n- Change FrameHeader aux from signed \"i\" to unsigned \"I\"; add aux_to_signed()\n- Guard memfd_create import with sys.platform and try/except for non-Linux\n- Reinterpret unsigned aux as signed status in _dispatch_response()\n- Add @skipif(not linux) to integration tests\n- Assert ring capacity is power of two in _SpscRingBase.__init__\n\n* Fix 6 Copilot review issues in Python transport\n\n1. connect(): join existing rx_thread before starting new one to prevent\n   thread leaks on reconnect (mirrors C++ ClientBase::connect).\n2. call(): register pending entry BEFORE write_frame, not after; erase on\n   write failure (matches C++ ordering).\n3. call(): skip seq numbers that collide with in-flight calls after\n   wraparound (mirrors C++ PR #23 fix).\n4. _rx_loop(): fail all pending calls with IPC_ERR_DISCONNECTED and set\n   _connected=False when loop exits on socket error (matches C++\n   receiverLoop exit behavior).\n5. _rx_loop(): add version check — skip frames with mismatched protocol\n   version before dispatching (matches C++ behavior).\n6. ringbuffer.py: replace assert with ValueError raise so the check is\n   not stripped in optimized Python (-O).\n\n* Fix minor review findings on PR #37\n\n- Remove redundant 512KB zero-write; memfd_create pages are already\n  zero-filled by the kernel.\n- Add all IPC error constants to __all__ in __init__.py so they are\n  part of the public API.\n- Add mark_disconnected() method to AetherTransport for proper\n  encapsulation instead of poking _connected directly from _rx_loop.\n\n* Fix 3 Copilot review issues: connect guard, send_signal safety, payload_bytes sync\n\n- connect() returns False immediately if already connected, matching\n  C++ ClientBase::connect() which checks m_running first.\n- send_signal() in call() is now wrapped in try/except; on\n  OSError/BrokenPipeError the pending entry is erased and\n  IPC_ERR_DISCONNECTED is returned.\n- write_frame() sets header.payload_bytes = len(payload) before\n  serializing, preventing header/payload length mismatches.\n\n* Fix review findings: MSG_NOSIGNAL, corrupt frame handling, header mutation",
          "timestamp": "2026-03-26T23:41:05+11:00",
          "tree_id": "a19532684b8ba7ede793b664bda75b5f1ca8a379",
          "url": "https://github.com/Mrunmoy/aether/commit/9d241d46cdd08b2778701eae2a95dee898ae11bb"
        },
        "date": 1774529004998,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 73342.68993839856,
            "unit": "ns/iter",
            "extra": "iterations: 9740\ncpu: 73122.24743326489 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 72606.11663066933,
            "unit": "ns/iter",
            "extra": "iterations: 9723\ncpu: 72396.95412938394 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 72584.74497474381,
            "unit": "ns/iter",
            "extra": "iterations: 9701\ncpu: 72353.60973095556 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 72962.17578980385,
            "unit": "ns/iter",
            "extra": "iterations: 9591\ncpu: 72746.7598790533 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 73410.3921527276,
            "unit": "ns/iter",
            "extra": "iterations: 9481\ncpu: 73300.97373694758 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2939.2588554138247,
            "unit": "ns/iter",
            "extra": "iterations: 237623\ncpu: 2938.6470754093652 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 2970.422498714976,
            "unit": "ns/iter",
            "extra": "iterations: 235409\ncpu: 2970.103747095484 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3039.801550172305,
            "unit": "ns/iter",
            "extra": "iterations: 230426\ncpu: 3039.225933705401 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4260.483601400094,
            "unit": "ns/iter",
            "extra": "iterations: 163703\ncpu: 4260.14110920386 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7382.000595182488,
            "unit": "ns/iter",
            "extra": "iterations: 85688\ncpu: 7380.766571748671 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2937.0004956067523,
            "unit": "ns/iter",
            "extra": "iterations: 238092\ncpu: 2936.8650605648213 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 2979.15564359467,
            "unit": "ns/iter",
            "extra": "iterations: 235249\ncpu: 2978.79050282892 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3041.868570485858,
            "unit": "ns/iter",
            "extra": "iterations: 230344\ncpu: 3041.6785546834317 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 4267.327643829744,
            "unit": "ns/iter",
            "extra": "iterations: 164865\ncpu: 4266.867182240012 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.55838570316921,
            "unit": "ns/iter",
            "extra": "iterations: 447890144\ncpu: 1.5583536685281472 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 97.27935119789521,
            "unit": "us/iter",
            "extra": "iterations: 7221\ncpu: 97.26811992798787 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.16156058039095,
            "unit": "us/iter",
            "extra": "iterations: 36596\ncpu: 19.160691414362237 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 42.81173012876838,
            "unit": "us/iter",
            "extra": "iterations: 53507\ncpu: 13.183257854112583 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 43.442094535598606,
            "unit": "us/iter",
            "extra": "iterations: 54974\ncpu: 13.738940353621693 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 43.80803002181316,
            "unit": "us/iter",
            "extra": "iterations: 52262\ncpu: 13.61708520531169 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 52.991677097557115,
            "unit": "us/iter",
            "extra": "iterations: 52728\ncpu: 14.305696309361226 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 43.66697287383295,
            "unit": "us/iter",
            "extra": "iterations: 54523\ncpu: 13.478481026355869 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 531.1072141342735,
            "unit": "us/iter",
            "extra": "iterations: 1415\ncpu: 484.24095335688946 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 43.571998827162865,
            "unit": "us/iter",
            "extra": "iterations: 51158\ncpu: 12.657465596778641 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 87.31667457653283,
            "unit": "us/iter",
            "extra": "iterations: 25917\ncpu: 26.265310028166923 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 177.32693295968994,
            "unit": "us/iter",
            "extra": "iterations: 13917\ncpu: 53.74971243802533 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 354.88431021953096,
            "unit": "us/iter",
            "extra": "iterations: 6605\ncpu: 107.43716532929598 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.6294553129863223,
            "unit": "us/iter",
            "extra": "iterations: 426041\ncpu: 1.6291533068413608 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.6243425969481313,
            "unit": "us/iter",
            "extra": "iterations: 431545\ncpu: 1.6241277224854818 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.6246641102551147,
            "unit": "us/iter",
            "extra": "iterations: 431073\ncpu: 1.6244192746936124 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 2.432869088353732,
            "unit": "us/iter",
            "extra": "iterations: 283191\ncpu: 2.4325672320094918 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.6309362250340778,
            "unit": "us/iter",
            "extra": "iterations: 431188\ncpu: 1.6305202974108783 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 5.261042599845831,
            "unit": "us/iter",
            "extra": "iterations: 119484\ncpu: 5.2584129088413665 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 16.798120809210566,
            "unit": "us/iter",
            "extra": "iterations: 45427\ncpu: 15.308542452726398 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 37.44916634580369,
            "unit": "us/iter",
            "extra": "iterations: 19219\ncpu: 35.92570752900781 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 77.49351405064851,
            "unit": "us/iter",
            "extra": "iterations: 9359\ncpu: 73.61031349503145 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.6397703193231972,
            "unit": "us/iter",
            "extra": "iterations: 420671\ncpu: 1.6395805011517295 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 43.29977991719196,
            "unit": "us/iter",
            "extra": "iterations: 54584\ncpu: 13.599680748937397 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 43.47532277596634,
            "unit": "us/iter",
            "extra": "iterations: 53675\ncpu: 13.140017941313541 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 52.56631639487494,
            "unit": "us/iter",
            "extra": "iterations: 48076\ncpu: 14.650954759131455 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 79.11852423166366,
            "unit": "us/iter",
            "extra": "iterations: 39143\ncpu: 17.17890634340752 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 19.79660543428195,
            "unit": "us/iter",
            "extra": "iterations: 54285\ncpu: 12.253565312701515 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 17.75783454342613,
            "unit": "us/iter",
            "extra": "iterations: 61841\ncpu: 10.999024158729593 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 21.747134775406437,
            "unit": "us/iter",
            "extra": "iterations: 51315\ncpu: 14.298119925947617 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 40.43258435371604,
            "unit": "us/iter",
            "extra": "iterations: 26869\ncpu: 27.48241631620068 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 26.470728200188674,
            "unit": "us/iter",
            "extra": "iterations: 50872\ncpu: 12.644722715835808 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 28.33120363494953,
            "unit": "us/iter",
            "extra": "iterations: 56562\ncpu: 13.639597415225762 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 30.75827363198792,
            "unit": "us/iter",
            "extra": "iterations: 33863\ncpu: 17.671060301804193 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 43.093388839062975,
            "unit": "us/iter",
            "extra": "iterations: 20070\ncpu: 30.61521938216263 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 50.15770792567897,
            "unit": "us/iter",
            "extra": "iterations: 22282\ncpu: 31.260260748586305 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 47.565273421965934,
            "unit": "us/iter",
            "extra": "iterations: 25649\ncpu: 28.918752894849607 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 49.93578672837866,
            "unit": "us/iter",
            "extra": "iterations: 19229\ncpu: 31.60073331946512 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 61.73883984321145,
            "unit": "us/iter",
            "extra": "iterations: 14542\ncpu: 45.018784211250065 us\nthreads: 1"
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
          "id": "ae54561ea93b69bd79b23c1d4a91e1ac7347a499",
          "message": "Add ipcgen --backend aether_lite for C99 MCU dispatch tables (#38)\n\n* Add ipcgen --backend aether_lite for C99 MCU dispatch tables\n\nNew backend generates C99-compatible .h and .c files from IDL for\nbare-metal firmware using the aether-lite runtime. The header contains\nservice/method/notification ID macros, extern handler declarations,\na static inline register function, and notification sender helpers.\nThe .c file contains dispatch wrappers that unmarshal request bytes,\ncall the user handler, and marshal response bytes using memcpy\n(no alignment assumptions).\n\n* Fix struct marshaling to use field-by-field memcpy instead of sizeof\n\nAddresses blocking review item #3 on PR #38: struct params were using\nsizeof(StructType) for wire size, which includes compiler padding. Now\nmarshals/unmarshals each struct field individually with computed offsets,\nand calculates wire size as the sum of field sizes. This applies to\ndispatch wrappers ([in]/[out] params) and notification senders.\n\n* Fix Copilot review issues: zero-method guard, notification preprocessor guard, inline types, cast safety\n\n- Guard zero-method services: register returns AL_ERR_INVALID_ARGUMENT\n  instead of emitting a zero-length array (comment #1)\n- Wrap notification senders in #if AL_ENABLE_NOTIFICATIONS (comment #2)\n- Emit C99 enum #defines and struct typedefs inline in the header so\n  IDLs with user types compile without a separate Types.h (comment #3)\n- Strengthen pointer-cast regex in test to catch spaceless casts (comment #4)\n- Add #if METHOD_COUNT > AL_MAX_METHODS compile-time guard and cast\n  method count to uint8_t in al_register_service call (comment #5)\n\n* Fix array-of-structs marshal/unmarshal to use field-by-field loops\n\nWhen a struct field or parameter is an array of structs (e.g.\nDeviceInfo[4] devices), a single memcpy using the total wire size\nis incorrect because sizeof(element) may include compiler padding.\nGenerate a for-loop that marshals/unmarshals each array element\nfield-by-field instead. Primitive and enum arrays still use memcpy.\n\nFixed in all four code paths:\n- _emit_struct_marshal (struct fields)\n- _emit_struct_unmarshal (struct fields)\n- _emit_notify_sender (notification params)\n- _emit_dispatch_wrapper ([in] unmarshal and [out] marshal)\n\n* Match emitter API: return strings from aether_lite emitters\n\nChange emit_aether_lite_h/emit_aether_lite_c to return str instead of\nwriting to a TextIO stream, matching the pattern used by all other\nemitters. Removes the StringIO wrapper in __main__.py and the stale\n`import io` from both __main__.py and the test file.\n\n* Fix enum wire size and prefix enum #define macros\n\n- Add _is_enum_type() helper to recognize IDL enum types\n- _struct_wire_size_int() and _wire_size_int() now return 4 for enum\n  types (uint32 on the wire) instead of None, avoiding sizeof() fallback\n- _emit_struct_marshal/unmarshal scalar branches handle enums with\n  explicit 4-byte memcpy instead of sizeof(EnumName)\n- Prefix enum value #define macros with UPPER_SNAKE enum type name\n  (e.g. #define DEVICE_STATUS_ONLINE 0) to prevent cross-service\n  collisions\n- Add TestEnumWireSize and TestEnumPrefixedDefines test classes\n\n* Fix 3 Copilot review issues: NULL guard, regex, NUL termination\n\n- Add NULL-pointer guard for array/struct pointer params in notification\n  senders to prevent memcpy crash on NULL dereference\n- Broaden pointer-cast regex in test to catch non-_t types like float,\n  double, int, char in addition to _t typedefs\n- Add explicit NUL termination after memcpy for string[N] struct fields\n  in _emit_struct_unmarshal to prevent buffer over-read\n\n* Fix review findings: struct NULL guard, nested loop variable shadowing",
          "timestamp": "2026-03-26T23:46:28+11:00",
          "tree_id": "a08cf132979e2f132fc66b5c42f9128c0854cbc4",
          "url": "https://github.com/Mrunmoy/aether/commit/ae54561ea93b69bd79b23c1d4a91e1ac7347a499"
        },
        "date": 1774529333172,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 61596.10553512046,
            "unit": "ns/iter",
            "extra": "iterations: 11418\ncpu: 61564.98922753547 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 61957.67466147532,
            "unit": "ns/iter",
            "extra": "iterations: 11299\ncpu: 61910.30303566686 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 61735.75857810682,
            "unit": "ns/iter",
            "extra": "iterations: 11337\ncpu: 61700.71129928552 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 62105.23818845096,
            "unit": "ns/iter",
            "extra": "iterations: 11239\ncpu: 62074.10828365513 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 64267.48525345562,
            "unit": "ns/iter",
            "extra": "iterations: 10850\ncpu: 64237.19566820279 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2961.0802726320276,
            "unit": "ns/iter",
            "extra": "iterations: 236656\ncpu: 2960.7984078155623 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 3020.1060170415185,
            "unit": "ns/iter",
            "extra": "iterations: 228501\ncpu: 3019.935742075527 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3086.8138167262964,
            "unit": "ns/iter",
            "extra": "iterations: 226841\ncpu: 3086.7048813926936 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4079.748164152376,
            "unit": "ns/iter",
            "extra": "iterations: 157829\ncpu: 4079.4393045638053 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7224.786508447721,
            "unit": "ns/iter",
            "extra": "iterations: 96238\ncpu: 7224.354963735737 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2961.713667520275,
            "unit": "ns/iter",
            "extra": "iterations: 236634\ncpu: 2961.518902609094 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 3082.808848477547,
            "unit": "ns/iter",
            "extra": "iterations: 231068\ncpu: 3082.6370202710905 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3096.9535795665442,
            "unit": "ns/iter",
            "extra": "iterations: 226452\ncpu: 3096.8703301361847 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 4011.9057654075064,
            "unit": "ns/iter",
            "extra": "iterations: 160960\ncpu: 4011.623086481106 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.6041942246380398,
            "unit": "ns/iter",
            "extra": "iterations: 448513602\ncpu: 1.6041321440235812 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 97.13358322938382,
            "unit": "us/iter",
            "extra": "iterations: 7215\ncpu: 97.12697532917525 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 20.11938349040313,
            "unit": "us/iter",
            "extra": "iterations: 34804\ncpu: 20.118044879898843 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 46.690048824854756,
            "unit": "us/iter",
            "extra": "iterations: 48377\ncpu: 14.10032513384456 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 47.036313215144126,
            "unit": "us/iter",
            "extra": "iterations: 48679\ncpu: 14.240745208406087 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 48.27345968534906,
            "unit": "us/iter",
            "extra": "iterations: 48816\ncpu: 14.54215759177316 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 56.01532951945076,
            "unit": "us/iter",
            "extra": "iterations: 45448\ncpu: 15.359012277768048 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 47.86074319327715,
            "unit": "us/iter",
            "extra": "iterations: 50575\ncpu: 14.051101473059765 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 500.9962006578941,
            "unit": "us/iter",
            "extra": "iterations: 1520\ncpu: 456.5679171052638 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 46.98407492681218,
            "unit": "us/iter",
            "extra": "iterations: 45431\ncpu: 13.975615218683286 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 97.16380087413286,
            "unit": "us/iter",
            "extra": "iterations: 24939\ncpu: 29.785164802117226 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 195.66562353203474,
            "unit": "us/iter",
            "extra": "iterations: 13454\ncpu: 59.74629515385772 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 387.94981380101785,
            "unit": "us/iter",
            "extra": "iterations: 6101\ncpu: 115.26208015079472 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.352603396874729,
            "unit": "us/iter",
            "extra": "iterations: 452828\ncpu: 1.352430055120267 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.2229199114130949,
            "unit": "us/iter",
            "extra": "iterations: 571642\ncpu: 1.2228637223996848 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.29418921825708,
            "unit": "us/iter",
            "extra": "iterations: 541137\ncpu: 1.29415450061629 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 0.010501537767981328,
            "unit": "us/iter",
            "extra": "iterations: 53987013\ncpu: 0.010500395159850698 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.2290275154090777,
            "unit": "us/iter",
            "extra": "iterations: 568009\ncpu: 1.2289091827770344 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 4.139106515377894,
            "unit": "us/iter",
            "extra": "iterations: 164718\ncpu: 4.1388784589419405 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 10.489611955012693,
            "unit": "us/iter",
            "extra": "iterations: 72020\ncpu: 9.590315953901698 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 30.411521374287997,
            "unit": "us/iter",
            "extra": "iterations: 25264\ncpu: 28.393753601963432 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 64.83853443665443,
            "unit": "us/iter",
            "extra": "iterations: 13837\ncpu: 59.60660836886631 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.2237470878976195,
            "unit": "us/iter",
            "extra": "iterations: 570550\ncpu: 1.2236829567960748 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 46.67189891792535,
            "unit": "us/iter",
            "extra": "iterations: 48980\ncpu: 13.563128766843587 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 47.15953127292364,
            "unit": "us/iter",
            "extra": "iterations: 51802\ncpu: 13.801739681865651 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 55.83512776999237,
            "unit": "us/iter",
            "extra": "iterations: 45668\ncpu: 14.942760970482635 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 83.87503106790827,
            "unit": "us/iter",
            "extra": "iterations: 36726\ncpu: 17.340832462015907 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 17.92929208852148,
            "unit": "us/iter",
            "extra": "iterations: 69633\ncpu: 8.914465741817791 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 20.247990987667425,
            "unit": "us/iter",
            "extra": "iterations: 75896\ncpu: 10.024860308843625 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 24.699638832997827,
            "unit": "us/iter",
            "extra": "iterations: 58646\ncpu: 14.479393053234727 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 41.03032388193831,
            "unit": "us/iter",
            "extra": "iterations: 26698\ncpu: 26.455114540415035 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 27.151558901832853,
            "unit": "us/iter",
            "extra": "iterations: 58971\ncpu: 12.222875209848834 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 26.89695650573134,
            "unit": "us/iter",
            "extra": "iterations: 59755\ncpu: 12.03913650740521 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 27.88983637392799,
            "unit": "us/iter",
            "extra": "iterations: 45934\ncpu: 14.21501436844173 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 61.64350595350487,
            "unit": "us/iter",
            "extra": "iterations: 21164\ncpu: 36.65009095634102 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 51.03769414614431,
            "unit": "us/iter",
            "extra": "iterations: 24770\ncpu: 30.989676059749545 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 28.65584143376057,
            "unit": "us/iter",
            "extra": "iterations: 33311\ncpu: 19.863685839512517 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 51.07333297297324,
            "unit": "us/iter",
            "extra": "iterations: 23125\ncpu: 31.555160086486776 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 59.777089860397794,
            "unit": "us/iter",
            "extra": "iterations: 13610\ncpu: 43.7995662747976 us\nthreads: 1"
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
          "id": "13650c34d6186a7006888d1cdbde9e8ea502488f",
          "message": "Update Vortex submodule to v1.2.1 (WFMO slot budget fix) (#48)",
          "timestamp": "2026-03-27T22:40:54+11:00",
          "tree_id": "b4598b8fe60670e3fbfd4d3ed2ce0cc0195013ae",
          "url": "https://github.com/Mrunmoy/aether/commit/13650c34d6186a7006888d1cdbde9e8ea502488f"
        },
        "date": 1774611790247,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 61711.314679142175,
            "unit": "ns/iter",
            "extra": "iterations: 11329\ncpu: 61614.87986583106 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 61862.11844573462,
            "unit": "ns/iter",
            "extra": "iterations: 11195\ncpu: 61771.99705225546 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 61974.37752848737,
            "unit": "ns/iter",
            "extra": "iterations: 11321\ncpu: 61886.240968112375 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 62554.191142398755,
            "unit": "ns/iter",
            "extra": "iterations: 11222\ncpu: 62464.296916770596 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 63472.30482695623,
            "unit": "ns/iter",
            "extra": "iterations: 10980\ncpu: 63446.557832422586 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 3282.430437573554,
            "unit": "ns/iter",
            "extra": "iterations: 213427\ncpu: 3282.2014459276497 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 3347.764462631676,
            "unit": "ns/iter",
            "extra": "iterations: 208693\ncpu: 3347.656193547462 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3367.321611633765,
            "unit": "ns/iter",
            "extra": "iterations: 208360\ncpu: 3367.06028508351 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4261.078550641515,
            "unit": "ns/iter",
            "extra": "iterations: 163907\ncpu: 4260.962326197167 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7285.39823949478,
            "unit": "ns/iter",
            "extra": "iterations: 96904\ncpu: 7284.7204449764695 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 3283.475904493378,
            "unit": "ns/iter",
            "extra": "iterations: 212467\ncpu: 3283.365553238855 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 3337.9661775275845,
            "unit": "ns/iter",
            "extra": "iterations: 210097\ncpu: 3337.7181777940696 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3360.753351051607,
            "unit": "ns/iter",
            "extra": "iterations: 207696\ncpu: 3360.602866689781 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 4259.943815442315,
            "unit": "ns/iter",
            "extra": "iterations: 164458\ncpu: 4259.793667684148 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.5589549465543968,
            "unit": "ns/iter",
            "extra": "iterations: 448033813\ncpu: 1.5588798607037253 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 98.02259236470339,
            "unit": "us/iter",
            "extra": "iterations: 7151\ncpu: 98.01676534750376 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.74527713599728,
            "unit": "us/iter",
            "extra": "iterations: 35405\ncpu: 19.744764270583254 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 46.837212411153274,
            "unit": "us/iter",
            "extra": "iterations: 51212\ncpu: 13.637092615012104 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 47.55946036343425,
            "unit": "us/iter",
            "extra": "iterations: 50408\ncpu: 14.32862823361371 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 48.5969790747563,
            "unit": "us/iter",
            "extra": "iterations: 49414\ncpu: 14.561732039502923 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 57.23793092133642,
            "unit": "us/iter",
            "extra": "iterations: 46324\ncpu: 14.903487522666403 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 47.53515134786056,
            "unit": "us/iter",
            "extra": "iterations: 48002\ncpu: 14.286726261405747 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 497.5985737282793,
            "unit": "us/iter",
            "extra": "iterations: 1553\ncpu: 450.90884095299265 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 47.22253839604605,
            "unit": "us/iter",
            "extra": "iterations: 48156\ncpu: 14.256112343217922 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 96.27072514946914,
            "unit": "us/iter",
            "extra": "iterations: 23249\ncpu: 29.01904253946398 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 192.63753043103475,
            "unit": "us/iter",
            "extra": "iterations: 11600\ncpu: 58.53309853448265 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 393.3632004697209,
            "unit": "us/iter",
            "extra": "iterations: 5961\ncpu: 118.72014510988097 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.2803494400006747,
            "unit": "us/iter",
            "extra": "iterations: 486072\ncpu: 1.280342498642174 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.2407677412466107,
            "unit": "us/iter",
            "extra": "iterations: 561925\ncpu: 1.2407436561818763 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.313915400275524,
            "unit": "us/iter",
            "extra": "iterations: 534919\ncpu: 1.3137833709402693 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 0.5940570534795903,
            "unit": "us/iter",
            "extra": "iterations: 946866\ncpu: 0.5939921646779998 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.258514194159618,
            "unit": "us/iter",
            "extra": "iterations: 560477\ncpu: 1.2583889169403906 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 4.307260012886256,
            "unit": "us/iter",
            "extra": "iterations: 162965\ncpu: 4.307024680146049 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 10.57646346090398,
            "unit": "us/iter",
            "extra": "iterations: 72744\ncpu: 9.76012725448145 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 29.876237314136933,
            "unit": "us/iter",
            "extra": "iterations: 25422\ncpu: 28.075139878844976 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 64.85922543090552,
            "unit": "us/iter",
            "extra": "iterations: 13228\ncpu: 59.80889000604778 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.2538836009502696,
            "unit": "us/iter",
            "extra": "iterations: 559008\ncpu: 1.25380343036236 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 47.05223648867982,
            "unit": "us/iter",
            "extra": "iterations: 49292\ncpu: 14.524442079850683 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 47.570873704465406,
            "unit": "us/iter",
            "extra": "iterations: 52488\ncpu: 14.22039546181985 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 57.40814123391249,
            "unit": "us/iter",
            "extra": "iterations: 44444\ncpu: 15.268509090090758 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 86.38229975410657,
            "unit": "us/iter",
            "extra": "iterations: 38228\ncpu: 18.684353327403905 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 21.31312220275018,
            "unit": "us/iter",
            "extra": "iterations: 74180\ncpu: 10.52412376651381 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 21.03097988625844,
            "unit": "us/iter",
            "extra": "iterations: 78951\ncpu: 10.625637002697893 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 25.867403272482335,
            "unit": "us/iter",
            "extra": "iterations: 63927\ncpu: 14.659380934503398 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 39.47345978271387,
            "unit": "us/iter",
            "extra": "iterations: 31019\ncpu: 24.94951452335662 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 27.511923082096484,
            "unit": "us/iter",
            "extra": "iterations: 59479\ncpu: 12.769715832478676 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 27.145240894351126,
            "unit": "us/iter",
            "extra": "iterations: 55817\ncpu: 12.478237597864487 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 30.70638077343858,
            "unit": "us/iter",
            "extra": "iterations: 42046\ncpu: 16.758638110640778 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 76.50356585594439,
            "unit": "us/iter",
            "extra": "iterations: 19201\ncpu: 42.780950002604094 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 46.79870706575074,
            "unit": "us/iter",
            "extra": "iterations: 26494\ncpu: 28.134811391258403 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 37.18145420922425,
            "unit": "us/iter",
            "extra": "iterations: 35149\ncpu: 25.607606873595273 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 46.636271567693576,
            "unit": "us/iter",
            "extra": "iterations: 18894\ncpu: 28.614820948448905 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 64.72661078079055,
            "unit": "us/iter",
            "extra": "iterations: 15305\ncpu: 47.491114929761274 us\nthreads: 1"
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
          "id": "82818dd469e1dfad14d30b14b306a55a2dd2585a",
          "message": "Update design docs and add Phase 5 P0 dev journal (#49)\n\n- Fix stale ms:: namespace refs in HLD and LLD (now vortex::, ouroboros::)\n- Add C API and Transport abstraction sections to HLD\n- Update LLD file index with all current source files\n- Update Windows RunLoop limitation to reflect Phase 5 status\n- Add P0 completion status table to phase5-runloop-plan.md\n- Add dev-journal-phase5-p0.md documenting the full P0 lifecycle\n- Add aether-vision.md to version control",
          "timestamp": "2026-03-28T07:33:03+11:00",
          "tree_id": "3d904f1dabe6caf1dd981ff3e2d380be6fb64dd8",
          "url": "https://github.com/Mrunmoy/aether/commit/82818dd469e1dfad14d30b14b306a55a2dd2585a"
        },
        "date": 1774643719093,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 62129.05050061442,
            "unit": "ns/iter",
            "extra": "iterations: 11386\ncpu: 62093.5847532057 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 62166.98508779674,
            "unit": "ns/iter",
            "extra": "iterations: 11333\ncpu: 62136.28765551929 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 64243.28915343986,
            "unit": "ns/iter",
            "extra": "iterations: 11340\ncpu: 64217.47257495591 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 62242.48671465323,
            "unit": "ns/iter",
            "extra": "iterations: 11253\ncpu: 62215.7104772061 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 64006.310549369635,
            "unit": "ns/iter",
            "extra": "iterations: 10958\ncpu: 63979.826428180335 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2963.1806552486382,
            "unit": "ns/iter",
            "extra": "iterations: 236002\ncpu: 2962.9681570495163 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 2990.811742001885,
            "unit": "ns/iter",
            "extra": "iterations: 233180\ncpu: 2990.67840295051 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3054.483497492893,
            "unit": "ns/iter",
            "extra": "iterations: 229147\ncpu: 3054.3546064316793 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 3909.9086071987726,
            "unit": "ns/iter",
            "extra": "iterations: 178920\ncpu: 3909.786921529169 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7003.567646501751,
            "unit": "ns/iter",
            "extra": "iterations: 100323\ncpu: 7003.054175014701 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2980.1503324966347,
            "unit": "ns/iter",
            "extra": "iterations: 236243\ncpu: 2980.091164605938 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 3024.965923508339,
            "unit": "ns/iter",
            "extra": "iterations: 234091\ncpu: 3024.7734855248555 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3060.258496237976,
            "unit": "ns/iter",
            "extra": "iterations: 228866\ncpu: 3060.0697089126375 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 3914.8287854497485,
            "unit": "ns/iter",
            "extra": "iterations: 179021\ncpu: 3914.6110121159045 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.5586036584467249,
            "unit": "ns/iter",
            "extra": "iterations: 448080055\ncpu: 1.5585271631873905 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 97.13663897364742,
            "unit": "us/iter",
            "extra": "iterations: 7210\ncpu: 97.12633134535372 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.12909820110456,
            "unit": "us/iter",
            "extra": "iterations: 36578\ncpu: 19.12738610640273 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 49.86949915242409,
            "unit": "us/iter",
            "extra": "iterations: 50143\ncpu: 15.25184937079953 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 49.939860646009144,
            "unit": "us/iter",
            "extra": "iterations: 47182\ncpu: 15.54529271756176 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 49.365525678273805,
            "unit": "us/iter",
            "extra": "iterations: 46810\ncpu: 15.420967955565073 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 58.86370712891068,
            "unit": "us/iter",
            "extra": "iterations: 41998\ncpu: 16.13683656364585 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 49.84493496018381,
            "unit": "us/iter",
            "extra": "iterations: 48724\ncpu: 14.996068651999003 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 493.1410155038778,
            "unit": "us/iter",
            "extra": "iterations: 1548\ncpu: 446.3101246770026 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 50.52742324341564,
            "unit": "us/iter",
            "extra": "iterations: 43892\ncpu: 15.129710197758062 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 99.88337134915535,
            "unit": "us/iter",
            "extra": "iterations: 23385\ncpu: 30.96766978832583 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 206.48595773788827,
            "unit": "us/iter",
            "extra": "iterations: 11476\ncpu: 62.485222289996635 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 411.99863914027037,
            "unit": "us/iter",
            "extra": "iterations: 5304\ncpu: 121.38118080693839 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.471116080860764,
            "unit": "us/iter",
            "extra": "iterations: 557551\ncpu: 1.4707944008709513 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.2633219872866408,
            "unit": "us/iter",
            "extra": "iterations: 559255\ncpu: 1.2632517599306254 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.302779746349039,
            "unit": "us/iter",
            "extra": "iterations: 536091\ncpu: 1.3027424746917995 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 2.024712309851732,
            "unit": "us/iter",
            "extra": "iterations: 350516\ncpu: 2.024677566787256 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.2576441065811514,
            "unit": "us/iter",
            "extra": "iterations: 557622\ncpu: 1.2575719824540632 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 4.371102686243921,
            "unit": "us/iter",
            "extra": "iterations: 254854\ncpu: 4.369396560383589 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 10.612992160980383,
            "unit": "us/iter",
            "extra": "iterations: 69524\ncpu: 9.722656507105498 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 30.81132421466995,
            "unit": "us/iter",
            "extra": "iterations: 24894\ncpu: 28.75210203261836 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 65.71809181560253,
            "unit": "us/iter",
            "extra": "iterations: 13037\ncpu: 60.66069855027971 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.2518965032107487,
            "unit": "us/iter",
            "extra": "iterations: 557969\ncpu: 1.2518063548333336 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 51.39609506234608,
            "unit": "us/iter",
            "extra": "iterations: 47958\ncpu: 15.454974540222707 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 50.019778684273454,
            "unit": "us/iter",
            "extra": "iterations: 50132\ncpu: 15.004705657065273 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 59.342699111979165,
            "unit": "us/iter",
            "extra": "iterations: 40427\ncpu: 16.66651960323558 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 86.5280741667505,
            "unit": "us/iter",
            "extra": "iterations: 39964\ncpu: 17.974093909518697 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 18.47614104730824,
            "unit": "us/iter",
            "extra": "iterations: 76329\ncpu: 8.974946114844945 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 17.24688184536954,
            "unit": "us/iter",
            "extra": "iterations: 81588\ncpu: 8.593738307104053 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 22.989187220562926,
            "unit": "us/iter",
            "extra": "iterations: 55245\ncpu: 12.896858937460415 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 36.5081214259192,
            "unit": "us/iter",
            "extra": "iterations: 32316\ncpu: 23.59524727689062 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 28.413018224584437,
            "unit": "us/iter",
            "extra": "iterations: 55694\ncpu: 12.674818059396017 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 28.179468466819195,
            "unit": "us/iter",
            "extra": "iterations: 65550\ncpu: 12.666262807017478 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 31.232074280293606,
            "unit": "us/iter",
            "extra": "iterations: 48492\ncpu: 16.19089806566039 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 66.72393406331868,
            "unit": "us/iter",
            "extra": "iterations: 21005\ncpu: 37.32999609616766 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 58.47773481621651,
            "unit": "us/iter",
            "extra": "iterations: 19017\ncpu: 35.69878776883849 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 32.841125402965055,
            "unit": "us/iter",
            "extra": "iterations: 24816\ncpu: 22.26306209703435 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 58.56979173298584,
            "unit": "us/iter",
            "extra": "iterations: 20733\ncpu: 36.40057078088066 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 62.18105821872999,
            "unit": "us/iter",
            "extra": "iterations: 15270\ncpu: 44.45407851997395 us\nthreads: 1"
          }
        ]
      }
    ]
  }
}