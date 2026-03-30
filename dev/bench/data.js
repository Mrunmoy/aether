window.BENCHMARK_DATA = {
  "lastUpdate": 1774845859944,
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
          "id": "8c463859a7daebd28bf0733b790fc89a21bb00dd",
          "message": "Harden handshake timeout, document pipe race, untrap Win32 tests (#50)\n\nD1: Handshake timeoutMs parameter (500ms for RunLoop). D7: Document ERROR_PIPE_CONNECTED. D9: Untrap 4 Win32 tests.",
          "timestamp": "2026-03-28T08:29:49+11:00",
          "tree_id": "129995d17bdb4bea8a22a726d1ad787adeabcab1",
          "url": "https://github.com/Mrunmoy/aether/commit/8c463859a7daebd28bf0733b790fc89a21bb00dd"
        },
        "date": 1774647129489,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 62462.318205898206,
            "unit": "ns/iter",
            "extra": "iterations: 11326\ncpu: 62431.096945082114 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 62370.399394316315,
            "unit": "ns/iter",
            "extra": "iterations: 11227\ncpu: 62326.60586087112 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 62262.40133570853,
            "unit": "ns/iter",
            "extra": "iterations: 11230\ncpu: 62225.591985752435 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 63157.013389648324,
            "unit": "ns/iter",
            "extra": "iterations: 11128\ncpu: 63111.270668583755 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 64720.147979985304,
            "unit": "ns/iter",
            "extra": "iterations: 10792\ncpu: 64684.02770570793 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2980.136056025013,
            "unit": "ns/iter",
            "extra": "iterations: 237608\ncpu: 2979.905011615771 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 2980.0371329070404,
            "unit": "ns/iter",
            "extra": "iterations: 234886\ncpu: 2979.7960116822646 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3038.0621244048907,
            "unit": "ns/iter",
            "extra": "iterations: 230634\ncpu: 3037.735316562171 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4397.038914407103,
            "unit": "ns/iter",
            "extra": "iterations: 159581\ncpu: 4396.645471578696 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7163.523251769704,
            "unit": "ns/iter",
            "extra": "iterations: 97756\ncpu: 7162.576650026603 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2941.927746259467,
            "unit": "ns/iter",
            "extra": "iterations: 238133\ncpu: 2941.6697223820315 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 2988.772486547273,
            "unit": "ns/iter",
            "extra": "iterations: 234896\ncpu: 2988.521400960421 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3038.4614613398207,
            "unit": "ns/iter",
            "extra": "iterations: 230405\ncpu: 3038.423801566811 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 4399.1742001430575,
            "unit": "ns/iter",
            "extra": "iterations: 159466\ncpu: 4398.885561812542 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.5618078122399435,
            "unit": "ns/iter",
            "extra": "iterations: 448339967\ncpu: 1.561730127887527 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 97.8000265425222,
            "unit": "us/iter",
            "extra": "iterations: 7196\ncpu: 97.78435200111184 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.173910295085538,
            "unit": "us/iter",
            "extra": "iterations: 36464\ncpu: 19.172114825581392 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 50.21248398222471,
            "unit": "us/iter",
            "extra": "iterations: 45231\ncpu: 15.412825208374787 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 51.0322710701823,
            "unit": "us/iter",
            "extra": "iterations: 42133\ncpu: 15.396548429971762 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 51.2055050693704,
            "unit": "us/iter",
            "extra": "iterations: 44976\ncpu: 15.742229522411945 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 60.934418384323955,
            "unit": "us/iter",
            "extra": "iterations: 43276\ncpu: 17.32605356317594 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 51.102461652437825,
            "unit": "us/iter",
            "extra": "iterations: 42519\ncpu: 15.29253980573397 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 513.3591280160776,
            "unit": "us/iter",
            "extra": "iterations: 1492\ncpu: 465.41490415549606 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 50.86542859725265,
            "unit": "us/iter",
            "extra": "iterations: 44256\ncpu: 15.66308654193781 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 103.77504798001551,
            "unit": "us/iter",
            "extra": "iterations: 23218\ncpu: 31.814827676802334 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 208.8609849033817,
            "unit": "us/iter",
            "extra": "iterations: 9936\ncpu: 65.6959525966185 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 421.11873812604136,
            "unit": "us/iter",
            "extra": "iterations: 5411\ncpu: 128.160821659582 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.265321387398396,
            "unit": "us/iter",
            "extra": "iterations: 562088\ncpu: 1.2652580005266034 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.242914481242563,
            "unit": "us/iter",
            "extra": "iterations: 563163\ncpu: 1.2427264486480782 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.3264127474747716,
            "unit": "us/iter",
            "extra": "iterations: 532529\ncpu: 1.3262703909082911 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 0.48503851303471734,
            "unit": "us/iter",
            "extra": "iterations: 1697659\ncpu: 0.484992231655473 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.2519354174166228,
            "unit": "us/iter",
            "extra": "iterations: 561142\ncpu: 1.25184714920644 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 3.39754493617471,
            "unit": "us/iter",
            "extra": "iterations: 247629\ncpu: 3.3974187958599296 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 10.601706629881669,
            "unit": "us/iter",
            "extra": "iterations: 69956\ncpu: 9.802428941048655 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 31.42174046614528,
            "unit": "us/iter",
            "extra": "iterations: 24413\ncpu: 29.42608130913841 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 67.7227674699772,
            "unit": "us/iter",
            "extra": "iterations: 12407\ncpu: 62.44077625533999 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.2452739747505175,
            "unit": "us/iter",
            "extra": "iterations: 561278\ncpu: 1.245183411785254 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 50.06052893559604,
            "unit": "us/iter",
            "extra": "iterations: 42249\ncpu: 15.11854725555636 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 51.69898641139101,
            "unit": "us/iter",
            "extra": "iterations: 47687\ncpu: 15.773821376895276 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 61.14180327079136,
            "unit": "us/iter",
            "extra": "iterations: 39440\ncpu: 16.680246348884335 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 87.40676599431764,
            "unit": "us/iter",
            "extra": "iterations: 35200\ncpu: 19.40871448863649 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 22.001921980620562,
            "unit": "us/iter",
            "extra": "iterations: 77814\ncpu: 10.766442876603195 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 20.738469081574447,
            "unit": "us/iter",
            "extra": "iterations: 70120\ncpu: 10.464729535082768 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 24.450186141233505,
            "unit": "us/iter",
            "extra": "iterations: 57451\ncpu: 13.749582722667956 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 43.23101411560329,
            "unit": "us/iter",
            "extra": "iterations: 27629\ncpu: 27.689039777045743 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 29.635061303116366,
            "unit": "us/iter",
            "extra": "iterations: 44125\ncpu: 13.89960201699717 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 30.392914198542556,
            "unit": "us/iter",
            "extra": "iterations: 48181\ncpu: 14.611923102467827 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 31.368544420920326,
            "unit": "us/iter",
            "extra": "iterations: 51012\ncpu: 16.184369854152056 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 60.67354227950313,
            "unit": "us/iter",
            "extra": "iterations: 20329\ncpu: 34.891116041123624 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 54.357514642160396,
            "unit": "us/iter",
            "extra": "iterations: 21001\ncpu: 32.841637398219106 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 31.35032821148328,
            "unit": "us/iter",
            "extra": "iterations: 24210\ncpu: 21.58163969434136 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 56.10713514569797,
            "unit": "us/iter",
            "extra": "iterations: 23027\ncpu: 34.732230512007945 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 63.3195866317181,
            "unit": "us/iter",
            "extra": "iterations: 15260\ncpu: 46.615557339449346 us\nthreads: 1"
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
          "id": "c459dea35ed989de8d4da0f36adb576c4a57e312",
          "message": "Update Vortex submodule to include PR #10 (#51)\n\nPulls in onError documentation and WFMO starvation sweep.",
          "timestamp": "2026-03-28T08:34:58+11:00",
          "tree_id": "0d3af6afc7a394175cfa64862969f3755c53f042",
          "url": "https://github.com/Mrunmoy/aether/commit/c459dea35ed989de8d4da0f36adb576c4a57e312"
        },
        "date": 1774647437037,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 61671.3110485576,
            "unit": "ns/iter",
            "extra": "iterations: 11368\ncpu: 61629.26486629134 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 61850.07681515188,
            "unit": "ns/iter",
            "extra": "iterations: 11404\ncpu: 61820.646001403016 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 61887.959992980985,
            "unit": "ns/iter",
            "extra": "iterations: 11398\ncpu: 61852.51210738726 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 62105.37745531743,
            "unit": "ns/iter",
            "extra": "iterations: 11302\ncpu: 62072.021323659515 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 64310.82547385657,
            "unit": "ns/iter",
            "extra": "iterations: 10921\ncpu: 64224.585111253546 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2992.1671927798297,
            "unit": "ns/iter",
            "extra": "iterations: 233790\ncpu: 2991.945626416868 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 3059.8404730127586,
            "unit": "ns/iter",
            "extra": "iterations: 230776\ncpu: 3059.508181093353 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3076.9095068967727,
            "unit": "ns/iter",
            "extra": "iterations: 227498\ncpu: 3076.5539784965163 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4003.757108530256,
            "unit": "ns/iter",
            "extra": "iterations: 174790\ncpu: 4003.097488414666 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7273.772308556972,
            "unit": "ns/iter",
            "extra": "iterations: 96073\ncpu: 7272.850603187159 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2957.1268100654793,
            "unit": "ns/iter",
            "extra": "iterations: 237008\ncpu: 2956.731667285495 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 2990.193694675624,
            "unit": "ns/iter",
            "extra": "iterations: 233961\ncpu: 2989.922153692282 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3045.184422058137,
            "unit": "ns/iter",
            "extra": "iterations: 229902\ncpu: 3044.9697175318206 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 4005.5435229413883,
            "unit": "ns/iter",
            "extra": "iterations: 175643\ncpu: 4005.363373433622 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.5587683646581179,
            "unit": "ns/iter",
            "extra": "iterations: 448677901\ncpu: 1.5587565298875712 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 97.1844069702868,
            "unit": "us/iter",
            "extra": "iterations: 7202\ncpu: 97.17723021382962 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.249628492460833,
            "unit": "us/iter",
            "extra": "iterations: 36543\ncpu: 19.248026243056103 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 48.64637843534166,
            "unit": "us/iter",
            "extra": "iterations: 47921\ncpu: 15.283164750318228 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 48.165856774989855,
            "unit": "us/iter",
            "extra": "iterations: 46989\ncpu: 15.263522079635607 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 50.99980188837246,
            "unit": "us/iter",
            "extra": "iterations: 47766\ncpu: 16.58053322446933 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 60.143611674455315,
            "unit": "us/iter",
            "extra": "iterations: 45364\ncpu: 16.821781081915155 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 49.10226847658967,
            "unit": "us/iter",
            "extra": "iterations: 47479\ncpu: 15.083743465532145 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 500.46471363929857,
            "unit": "us/iter",
            "extra": "iterations: 1547\ncpu: 454.87678797673084 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 49.72294457877925,
            "unit": "us/iter",
            "extra": "iterations: 41356\ncpu: 15.248532135603003 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 100.87693896753763,
            "unit": "us/iter",
            "extra": "iterations: 23381\ncpu: 30.666751379325163 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 202.454990449813,
            "unit": "us/iter",
            "extra": "iterations: 10471\ncpu: 62.78881529939846 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 404.32852753770544,
            "unit": "us/iter",
            "extra": "iterations: 5901\ncpu: 126.4534880528723 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.2438720787772426,
            "unit": "us/iter",
            "extra": "iterations: 566458\ncpu: 1.2438252209342948 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.2561496483970471,
            "unit": "us/iter",
            "extra": "iterations: 562993\ncpu: 1.2560338387866288 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.3086915091041804,
            "unit": "us/iter",
            "extra": "iterations: 537776\ncpu: 1.3085294583618465 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 2.0993864837871365,
            "unit": "us/iter",
            "extra": "iterations: 341427\ncpu: 2.0991399918577067 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.24072858628281,
            "unit": "us/iter",
            "extra": "iterations: 564533\ncpu: 1.2404600492796736 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 4.342182248147549,
            "unit": "us/iter",
            "extra": "iterations: 253720\ncpu: 4.342040426454357 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 10.662633998435023,
            "unit": "us/iter",
            "extra": "iterations: 71568\ncpu: 9.801793594902742 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 30.855281030349737,
            "unit": "us/iter",
            "extra": "iterations: 24613\ncpu: 28.828062812334966 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 67.10543008458303,
            "unit": "us/iter",
            "extra": "iterations: 13123\ncpu: 62.18627112702864 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.241852232063767,
            "unit": "us/iter",
            "extra": "iterations: 564500\ncpu: 1.2417901594331324 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 49.748367764174425,
            "unit": "us/iter",
            "extra": "iterations: 42065\ncpu: 15.753916486390057 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 49.07755660399172,
            "unit": "us/iter",
            "extra": "iterations: 43239\ncpu: 14.511962950114478 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 58.707380650034494,
            "unit": "us/iter",
            "extra": "iterations: 42367\ncpu: 16.202197158165475 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 85.74387612456741,
            "unit": "us/iter",
            "extra": "iterations: 39015\ncpu: 18.355206920415096 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 17.45485556959939,
            "unit": "us/iter",
            "extra": "iterations: 85446\ncpu: 8.571310055473617 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 18.955443184457963,
            "unit": "us/iter",
            "extra": "iterations: 68872\ncpu: 9.493362004878612 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 22.796843238811114,
            "unit": "us/iter",
            "extra": "iterations: 50006\ncpu: 12.93541139063311 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 32.7003633534511,
            "unit": "us/iter",
            "extra": "iterations: 31812\ncpu: 21.42328266063107 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 27.559549416671793,
            "unit": "us/iter",
            "extra": "iterations: 49629\ncpu: 12.744590279876734 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 28.63468530435736,
            "unit": "us/iter",
            "extra": "iterations: 56874\ncpu: 13.62520911136904 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 30.358877056662795,
            "unit": "us/iter",
            "extra": "iterations: 47103\ncpu: 15.951853533745192 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 58.59558756035466,
            "unit": "us/iter",
            "extra": "iterations: 19261\ncpu: 33.127078760189036 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 50.75878008654683,
            "unit": "us/iter",
            "extra": "iterations: 25651\ncpu: 30.19544614245037 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 31.47267112444618,
            "unit": "us/iter",
            "extra": "iterations: 34995\ncpu: 21.591570652950402 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 53.30806561968529,
            "unit": "us/iter",
            "extra": "iterations: 19994\ncpu: 33.10632974892448 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 63.312766562719816,
            "unit": "us/iter",
            "extra": "iterations: 15713\ncpu: 47.04637109399883 us\nthreads: 1"
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
          "id": "7c9b9008213fef35b57613c81db4810a19091bae",
          "message": "Add Phase 5 P1 hardening development journal (#52)",
          "timestamp": "2026-03-28T08:46:56+11:00",
          "tree_id": "9c281bae46004981e70fe04776482be6c062745e",
          "url": "https://github.com/Mrunmoy/aether/commit/7c9b9008213fef35b57613c81db4810a19091bae"
        },
        "date": 1774648151922,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 60185.94339954353,
            "unit": "ns/iter",
            "extra": "iterations: 11378\ncpu: 60157.501230444715 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 60633.71896460698,
            "unit": "ns/iter",
            "extra": "iterations: 11358\ncpu: 60598.85270294066 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 60523.76981132097,
            "unit": "ns/iter",
            "extra": "iterations: 11660\ncpu: 60489.9691252144 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 60622.318529335585,
            "unit": "ns/iter",
            "extra": "iterations: 11641\ncpu: 60582.19749162443 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 62439.544253632055,
            "unit": "ns/iter",
            "extra": "iterations: 11355\ncpu: 62406.97164244827 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2932.31150349917,
            "unit": "ns/iter",
            "extra": "iterations: 235198\ncpu: 2932.1768382384203 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 2963.6325552313733,
            "unit": "ns/iter",
            "extra": "iterations: 235102\ncpu: 2963.54789835901 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3093.5557791986107,
            "unit": "ns/iter",
            "extra": "iterations: 228042\ncpu: 3093.3682435691703 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4261.385307448216,
            "unit": "ns/iter",
            "extra": "iterations: 161946\ncpu: 4261.307299964185 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 8750.200101223414,
            "unit": "ns/iter",
            "extra": "iterations: 81009\ncpu: 8749.954400128383 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 3230.6680931434003,
            "unit": "ns/iter",
            "extra": "iterations: 167312\ncpu: 3230.581225494882 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 2884.416494079808,
            "unit": "ns/iter",
            "extra": "iterations: 242390\ncpu: 2884.293861132886 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 2938.215015808716,
            "unit": "ns/iter",
            "extra": "iterations: 239108\ncpu: 2938.2058274921815 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 3764.593720948801,
            "unit": "ns/iter",
            "extra": "iterations: 188245\ncpu: 3764.298403676056 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.4502655111029412,
            "unit": "ns/iter",
            "extra": "iterations: 481879095\ncpu: 1.4502409198722337 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 89.97582207556843,
            "unit": "us/iter",
            "extra": "iterations: 7728\ncpu: 89.9720852743271 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 18.2944240291962,
            "unit": "us/iter",
            "extra": "iterations: 38087\ncpu: 18.29382508467456 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 43.72930456824822,
            "unit": "us/iter",
            "extra": "iterations: 54441\ncpu: 12.780499145864345 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 45.22786405096847,
            "unit": "us/iter",
            "extra": "iterations: 50541\ncpu: 13.678425199343128 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 44.89494210537903,
            "unit": "us/iter",
            "extra": "iterations: 45341\ncpu: 13.23240195408134 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 54.12711702840942,
            "unit": "us/iter",
            "extra": "iterations: 52201\ncpu: 14.3245785904485 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 45.17634604120902,
            "unit": "us/iter",
            "extra": "iterations: 57366\ncpu: 13.214000557821699 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 484.79233752243744,
            "unit": "us/iter",
            "extra": "iterations: 1671\ncpu: 440.7031621783361 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 45.27116487031121,
            "unit": "us/iter",
            "extra": "iterations: 54091\ncpu: 13.327115472074869 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 90.74627278422692,
            "unit": "us/iter",
            "extra": "iterations: 27135\ncpu: 27.553108789386407 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 182.72343484702517,
            "unit": "us/iter",
            "extra": "iterations: 13499\ncpu: 54.54979798503598 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 370.6952845837776,
            "unit": "us/iter",
            "extra": "iterations: 6571\ncpu: 110.94704824227699 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.4186772018301683,
            "unit": "us/iter",
            "extra": "iterations: 579393\ncpu: 1.4185832258933069 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.2281482860250212,
            "unit": "us/iter",
            "extra": "iterations: 589361\ncpu: 1.2280972510905843 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.2353861201558423,
            "unit": "us/iter",
            "extra": "iterations: 550352\ncpu: 1.2352963884931836 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 1.951766882879612,
            "unit": "us/iter",
            "extra": "iterations: 363993\ncpu: 1.951651309228473 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.2244567307171859,
            "unit": "us/iter",
            "extra": "iterations: 554250\ncpu: 1.2243311447902572 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 3.6250777686423397,
            "unit": "us/iter",
            "extra": "iterations: 167844\ncpu: 3.6234502931293258 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 10.052158235394263,
            "unit": "us/iter",
            "extra": "iterations: 76323\ncpu: 9.25031171468625 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 29.032976603347947,
            "unit": "us/iter",
            "extra": "iterations: 26585\ncpu: 26.988612525860333 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 63.25750319459603,
            "unit": "us/iter",
            "extra": "iterations: 10956\ncpu: 58.12431151880221 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.2273636888444945,
            "unit": "us/iter",
            "extra": "iterations: 564708\ncpu: 1.227348578380327 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 44.37978128535643,
            "unit": "us/iter",
            "extra": "iterations: 53915\ncpu: 13.57888492998236 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 44.48028975861531,
            "unit": "us/iter",
            "extra": "iterations: 51536\ncpu: 12.973834426420316 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 52.93702588642965,
            "unit": "us/iter",
            "extra": "iterations: 48481\ncpu: 13.839044986695878 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 80.64233705763125,
            "unit": "us/iter",
            "extra": "iterations: 38486\ncpu: 17.888125448214915 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 18.058267830818316,
            "unit": "us/iter",
            "extra": "iterations: 83437\ncpu: 9.182979385644268 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 17.33563108357667,
            "unit": "us/iter",
            "extra": "iterations: 67148\ncpu: 8.74309991362364 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 21.307752993924527,
            "unit": "us/iter",
            "extra": "iterations: 51521\ncpu: 12.532560354030363 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 50.313413374864666,
            "unit": "us/iter",
            "extra": "iterations: 21189\ncpu: 32.83762574920965 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 26.381571659776082,
            "unit": "us/iter",
            "extra": "iterations: 62406\ncpu: 12.15448240553796 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 25.483109775938043,
            "unit": "us/iter",
            "extra": "iterations: 62081\ncpu: 11.751109598750022 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 29.22564915225457,
            "unit": "us/iter",
            "extra": "iterations: 53023\ncpu: 15.794310205005365 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 55.43720409095499,
            "unit": "us/iter",
            "extra": "iterations: 19702\ncpu: 35.829961882042284 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 45.426092395886755,
            "unit": "us/iter",
            "extra": "iterations: 23724\ncpu: 27.511097580509116 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 28.84789887048669,
            "unit": "us/iter",
            "extra": "iterations: 34174\ncpu: 20.136615731257674 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 47.07329289664295,
            "unit": "us/iter",
            "extra": "iterations: 25678\ncpu: 29.442507204610855 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 59.8994659578441,
            "unit": "us/iter",
            "extra": "iterations: 16083\ncpu: 43.289676055462316 us\nthreads: 1"
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
          "id": "d6ce511a5bc1a0d37fae051a7c73d1df65f0bff2",
          "message": "Add architecture guide, diagrams, and rewrite README (#53)\n\n* Add architecture guide, 17 diagrams, vision doc, and rewrite README\n\nAdd doc/architecture-guide.md — visual walkthrough of the entire\nframework using Excalidraw diagrams, covering layers, components,\nRPC flow, transports, codegen, and platform abstraction.\n\nAdd doc/aether-vision.md — project vision, architecture decisions,\nusage scenarios, platform requirements, and transport strategy.\n\nAdd doc/phase5-runloop-plan.md — Phase 5 implementation plan for\nWindows RunLoop completion with expert review findings.\n\nAdd CLAUDE.md — build commands, architecture overview, coding\nconventions, and testing patterns for Claude Code.\n\nAdd 14 Excalidraw diagrams with PNG exports across 7 categories:\noverview (3), runtime (4), framing (1), connection (1), platform (1),\ntransport (2), codegen (2).\n\nRewrite README with embedded diagrams, features list, IDL examples,\narchitecture section, transport comparison, and documentation links.\n\n* Fix review findings: badge URL, missing PNGs, wire format docs, README index\n\n- Fix CI badge URL to match actual repo name (ms-ipc, not aether)\n- Comment out image refs for missing PNG exports (aether-architecture,\n  runloop-integration) with TODO notes to export from Excalidraw\n- Correct FrameHeader description: version and flags are 2 bytes each,\n  remove erroneous '2 reserved bytes' claim\n- Add Phase 5 docs to README Documentation table\n\nCo-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>\n\n---------\n\nCo-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>",
          "timestamp": "2026-03-28T11:09:13+11:00",
          "tree_id": "e6c6aa2412b667ec67fc159a031c45ebabea103f",
          "url": "https://github.com/Mrunmoy/aether/commit/d6ce511a5bc1a0d37fae051a7c73d1df65f0bff2"
        },
        "date": 1774656698258,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 62240.25004360801,
            "unit": "ns/iter",
            "extra": "iterations: 11466\ncpu: 62208.67660910518 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 62366.36345634644,
            "unit": "ns/iter",
            "extra": "iterations: 11110\ncpu: 62332.00189018902 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 62359.51138088094,
            "unit": "ns/iter",
            "extra": "iterations: 11203\ncpu: 62317.7963045613 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 63110.01430370569,
            "unit": "ns/iter",
            "extra": "iterations: 11116\ncpu: 63081.20214105793 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 64844.469861474536,
            "unit": "ns/iter",
            "extra": "iterations: 10684\ncpu: 64753.13852489702 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2948.9925257243453,
            "unit": "ns/iter",
            "extra": "iterations: 237615\ncpu: 2948.718048103022 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 3003.455482298128,
            "unit": "ns/iter",
            "extra": "iterations: 234637\ncpu: 3003.1834024471846 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3042.5595372558187,
            "unit": "ns/iter",
            "extra": "iterations: 230192\ncpu: 3042.313451379719 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 3918.4298711101987,
            "unit": "ns/iter",
            "extra": "iterations: 178835\ncpu: 3917.943187854728 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7059.32992816915,
            "unit": "ns/iter",
            "extra": "iterations: 99261\ncpu: 7058.9135410685085 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2940.0292366158183,
            "unit": "ns/iter",
            "extra": "iterations: 237613\ncpu: 2939.684840475901 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 2976.2373703479802,
            "unit": "ns/iter",
            "extra": "iterations: 235438\ncpu: 2975.8951103899944 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3038.0117275089583,
            "unit": "ns/iter",
            "extra": "iterations: 230569\ncpu: 3037.5366332854805 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 3903.623913539785,
            "unit": "ns/iter",
            "extra": "iterations: 179597\ncpu: 3903.301118615571 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.5591977323160893,
            "unit": "ns/iter",
            "extra": "iterations: 448554226\ncpu: 1.5590213478447976 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 97.03483310249275,
            "unit": "us/iter",
            "extra": "iterations: 7220\ncpu: 97.0261493074793 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 18.893876799482232,
            "unit": "us/iter",
            "extra": "iterations: 37094\ncpu: 18.892209710465316 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 49.09559190419036,
            "unit": "us/iter",
            "extra": "iterations: 44171\ncpu: 15.930054538045361 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 50.343918602512026,
            "unit": "us/iter",
            "extra": "iterations: 43478\ncpu: 16.147740328441937 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 50.53450641419855,
            "unit": "us/iter",
            "extra": "iterations: 47161\ncpu: 15.81320701427028 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 57.410963934426306,
            "unit": "us/iter",
            "extra": "iterations: 40870\ncpu: 15.258340787863983 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 49.98126936644604,
            "unit": "us/iter",
            "extra": "iterations: 42522\ncpu: 16.249967193452772 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 492.2368067885127,
            "unit": "us/iter",
            "extra": "iterations: 1532\ncpu: 446.76339490861625 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 48.40571051965587,
            "unit": "us/iter",
            "extra": "iterations: 47416\ncpu: 14.74102446431581 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 95.53834179660552,
            "unit": "us/iter",
            "extra": "iterations: 21741\ncpu: 29.09210979255792 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 202.60914709580015,
            "unit": "us/iter",
            "extra": "iterations: 11931\ncpu: 63.91762643533643 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 401.58565301886864,
            "unit": "us/iter",
            "extra": "iterations: 5300\ncpu: 121.57908773584916 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.2885401594636268,
            "unit": "us/iter",
            "extra": "iterations: 539935\ncpu: 1.2884569938974157 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.2631599237747568,
            "unit": "us/iter",
            "extra": "iterations: 569365\ncpu: 1.2630937096590065 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.2937035231703917,
            "unit": "us/iter",
            "extra": "iterations: 545503\ncpu: 1.2935645303508891 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 1.6570436589259359,
            "unit": "us/iter",
            "extra": "iterations: 353284\ncpu: 1.6568203343485677 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.2430809561648537,
            "unit": "us/iter",
            "extra": "iterations: 562539\ncpu: 1.2429614977806032 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 3.827458413059905,
            "unit": "us/iter",
            "extra": "iterations: 165148\ncpu: 3.827373695109841 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 10.695283426273255,
            "unit": "us/iter",
            "extra": "iterations: 70117\ncpu: 9.844154128100191 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 30.937781993229915,
            "unit": "us/iter",
            "extra": "iterations: 24513\ncpu: 28.952567780361488 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 66.65082688706154,
            "unit": "us/iter",
            "extra": "iterations: 12281\ncpu: 61.54716374888047 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.248426804308183,
            "unit": "us/iter",
            "extra": "iterations: 547703\ncpu: 1.2483533447872317 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 49.52985904520509,
            "unit": "us/iter",
            "extra": "iterations: 48136\ncpu: 15.187732902609273 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 50.031855371811034,
            "unit": "us/iter",
            "extra": "iterations: 46381\ncpu: 15.902823462193538 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 57.3340023819271,
            "unit": "us/iter",
            "extra": "iterations: 41563\ncpu: 15.57883206217077 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 85.40235921995252,
            "unit": "us/iter",
            "extra": "iterations: 37690\ncpu: 18.398140249403067 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 19.56413305692712,
            "unit": "us/iter",
            "extra": "iterations: 70669\ncpu: 9.637681939747267 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 19.033921559883233,
            "unit": "us/iter",
            "extra": "iterations: 80724\ncpu: 9.374556278182501 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 27.04474960294318,
            "unit": "us/iter",
            "extra": "iterations: 47852\ncpu: 15.449055797040863 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 42.59300252698465,
            "unit": "us/iter",
            "extra": "iterations: 27701\ncpu: 27.238825457564786 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 30.088825771419046,
            "unit": "us/iter",
            "extra": "iterations: 58010\ncpu: 14.794253594207849 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 31.046782281939727,
            "unit": "us/iter",
            "extra": "iterations: 52692\ncpu: 15.409349407879807 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 30.624619268506443,
            "unit": "us/iter",
            "extra": "iterations: 52386\ncpu: 15.883290917420682 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 54.40915781895115,
            "unit": "us/iter",
            "extra": "iterations: 20834\ncpu: 30.965847220888882 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 54.44394557373788,
            "unit": "us/iter",
            "extra": "iterations: 21203\ncpu: 33.324315474225344 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 31.000721741943174,
            "unit": "us/iter",
            "extra": "iterations: 34008\ncpu: 21.29266143260421 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 56.35124162720599,
            "unit": "us/iter",
            "extra": "iterations: 23230\ncpu: 35.209739474816836 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 64.88138643273155,
            "unit": "us/iter",
            "extra": "iterations: 15788\ncpu: 48.42050126678509 us\nthreads: 1"
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
          "id": "353003938612681ecf8215d9ddf481ab2cc2342e",
          "message": "Update Vortex and Ouroboros submodules to latest docs (#54)",
          "timestamp": "2026-03-28T11:12:12+11:00",
          "tree_id": "c46c1d58ebdc4fe422ea20b098ee2dbbe0f7cd46",
          "url": "https://github.com/Mrunmoy/aether/commit/353003938612681ecf8215d9ddf481ab2cc2342e"
        },
        "date": 1774656873230,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 62244.5954124402,
            "unit": "ns/iter",
            "extra": "iterations: 11335\ncpu: 62199.587207763565 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 61913.067772678434,
            "unit": "ns/iter",
            "extra": "iterations: 11332\ncpu: 61868.9849099894 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 61900.05597806928,
            "unit": "ns/iter",
            "extra": "iterations: 11308\ncpu: 61853.36655465158 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 62661.1088325235,
            "unit": "ns/iter",
            "extra": "iterations: 11118\ncpu: 62629.295107033606 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 63752.363934724744,
            "unit": "ns/iter",
            "extra": "iterations: 10969\ncpu: 63716.33184428846 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2962.545988498089,
            "unit": "ns/iter",
            "extra": "iterations: 236657\ncpu: 2961.8414076067893 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 3008.370291665582,
            "unit": "ns/iter",
            "extra": "iterations: 232938\ncpu: 3008.1074191415732 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3068.3540845180783,
            "unit": "ns/iter",
            "extra": "iterations: 229513\ncpu: 3068.0991839242242 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4016.213822435491,
            "unit": "ns/iter",
            "extra": "iterations: 174224\ncpu: 4015.9865977132895 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7145.813378698746,
            "unit": "ns/iter",
            "extra": "iterations: 97872\ncpu: 7145.102450138959 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2983.167801601929,
            "unit": "ns/iter",
            "extra": "iterations: 234962\ncpu: 2982.8983920804158 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 3045.889556730796,
            "unit": "ns/iter",
            "extra": "iterations: 229928\ncpu: 3045.573014160955 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3072.461162638459,
            "unit": "ns/iter",
            "extra": "iterations: 227603\ncpu: 3072.230818574453 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 4018.457373503222,
            "unit": "ns/iter",
            "extra": "iterations: 174293\ncpu: 4018.3314189324847 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.5621747532025179,
            "unit": "ns/iter",
            "extra": "iterations: 448993660\ncpu: 1.562044105478015 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 97.47090761096412,
            "unit": "us/iter",
            "extra": "iterations: 7187\ncpu: 97.46128788089611 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.242126432044888,
            "unit": "us/iter",
            "extra": "iterations: 36399\ncpu: 19.2397675760323 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 46.45806765557954,
            "unit": "us/iter",
            "extra": "iterations: 45909\ncpu: 13.560729747979687 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 47.41442029960969,
            "unit": "us/iter",
            "extra": "iterations: 52001\ncpu: 14.199321897655821 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 47.48393387761692,
            "unit": "us/iter",
            "extra": "iterations: 49484\ncpu: 14.42875606256564 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 56.92598207582986,
            "unit": "us/iter",
            "extra": "iterations: 47422\ncpu: 15.11221477373372 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 46.20548346100277,
            "unit": "us/iter",
            "extra": "iterations: 45952\ncpu: 14.284713048398306 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 490.6990458830917,
            "unit": "us/iter",
            "extra": "iterations: 1591\ncpu: 446.2565556253944 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 46.39703572914823,
            "unit": "us/iter",
            "extra": "iterations: 50463\ncpu: 14.418235796524115 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 95.2038861520097,
            "unit": "us/iter",
            "extra": "iterations: 25130\ncpu: 29.345178949462806 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 189.9034977746545,
            "unit": "us/iter",
            "extra": "iterations: 12807\ncpu: 57.10343671429702 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 391.48085299641394,
            "unit": "us/iter",
            "extra": "iterations: 5857\ncpu: 117.2166223322526 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.2441319201293886,
            "unit": "us/iter",
            "extra": "iterations: 558353\ncpu: 1.244078647378991 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.2688385119562302,
            "unit": "us/iter",
            "extra": "iterations: 555360\ncpu: 1.268573766565832 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.3171161308146104,
            "unit": "us/iter",
            "extra": "iterations: 528473\ncpu: 1.3169393952008892 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 0.36110386681285445,
            "unit": "us/iter",
            "extra": "iterations: 3256170\ncpu: 0.3610270726037041 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.2644232590256423,
            "unit": "us/iter",
            "extra": "iterations: 535835\ncpu: 1.264346819450014 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 4.274794386922937,
            "unit": "us/iter",
            "extra": "iterations: 161872\ncpu: 4.2735575269348685 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 10.288109987461047,
            "unit": "us/iter",
            "extra": "iterations: 73372\ncpu: 9.48463532410187 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 29.724553556502215,
            "unit": "us/iter",
            "extra": "iterations: 25123\ncpu: 27.94312132309035 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 65.27651235596896,
            "unit": "us/iter",
            "extra": "iterations: 13799\ncpu: 59.78921450829781 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.2583008176196802,
            "unit": "us/iter",
            "extra": "iterations: 557105\ncpu: 1.2582352572674858 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 46.6261902837937,
            "unit": "us/iter",
            "extra": "iterations: 51481\ncpu: 14.315345117616335 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 47.74188011707962,
            "unit": "us/iter",
            "extra": "iterations: 48514\ncpu: 14.386376035783545 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 56.747083964673706,
            "unit": "us/iter",
            "extra": "iterations: 46198\ncpu: 15.4615574050824 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 84.30818226652988,
            "unit": "us/iter",
            "extra": "iterations: 37917\ncpu: 18.169591739852937 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 18.533457454254474,
            "unit": "us/iter",
            "extra": "iterations: 70007\ncpu: 9.205774693959254 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 17.988811077932933,
            "unit": "us/iter",
            "extra": "iterations: 81044\ncpu: 8.933066729184164 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 24.46453990031069,
            "unit": "us/iter",
            "extra": "iterations: 60789\ncpu: 13.948749115793937 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 36.16669053698809,
            "unit": "us/iter",
            "extra": "iterations: 26630\ncpu: 23.48786774314677 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 26.947748380776193,
            "unit": "us/iter",
            "extra": "iterations: 55119\ncpu: 12.21657825795095 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 27.065879930100387,
            "unit": "us/iter",
            "extra": "iterations: 57225\ncpu: 12.22194663171691 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 29.363214905779934,
            "unit": "us/iter",
            "extra": "iterations: 42507\ncpu: 15.135957606982279 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 60.58234243688265,
            "unit": "us/iter",
            "extra": "iterations: 22775\ncpu: 34.79593497255767 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 51.205559959676805,
            "unit": "us/iter",
            "extra": "iterations: 24800\ncpu: 31.242484717741927 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 38.43345514950091,
            "unit": "us/iter",
            "extra": "iterations: 25585\ncpu: 26.568287824897304 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 50.78368597358813,
            "unit": "us/iter",
            "extra": "iterations: 19613\ncpu: 31.663495487686635 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 62.258444949362946,
            "unit": "us/iter",
            "extra": "iterations: 15404\ncpu: 45.922676382757665 us\nthreads: 1"
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
          "id": "04f2d70641f9e7516226292c8fa9f6659e6eadcb",
          "message": "Update Ouroboros submodule with diagram exports and doc/ consolidation (#55)",
          "timestamp": "2026-03-28T11:24:01+11:00",
          "tree_id": "06b40469a61bcdcded7ba8178157e1b555ca4a4f",
          "url": "https://github.com/Mrunmoy/aether/commit/04f2d70641f9e7516226292c8fa9f6659e6eadcb"
        },
        "date": 1774657581460,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 61627.11110141345,
            "unit": "ns/iter",
            "extra": "iterations: 11458\ncpu: 61597.14461511608 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 62010.372848613246,
            "unit": "ns/iter",
            "extra": "iterations: 11388\ncpu: 61978.79206181946 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 62430.20005322866,
            "unit": "ns/iter",
            "extra": "iterations: 11272\ncpu: 62399.41829311568 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 63239.8515681328,
            "unit": "ns/iter",
            "extra": "iterations: 11096\ncpu: 63209.37833453496 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 64297.51392475082,
            "unit": "ns/iter",
            "extra": "iterations: 10844\ncpu: 64270.176964219834 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2940.47883229491,
            "unit": "ns/iter",
            "extra": "iterations: 237697\ncpu: 2940.428730694961 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 2998.3948406833115,
            "unit": "ns/iter",
            "extra": "iterations: 233403\ncpu: 2998.3433032137555 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3087.5259883418044,
            "unit": "ns/iter",
            "extra": "iterations: 226794\ncpu: 3087.487243930613 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4102.459248190824,
            "unit": "ns/iter",
            "extra": "iterations: 170655\ncpu: 4102.254290820663 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7283.020356578124,
            "unit": "ns/iter",
            "extra": "iterations: 96136\ncpu: 7282.840371973039 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2939.326002004424,
            "unit": "ns/iter",
            "extra": "iterations: 237474\ncpu: 2939.2622392346098 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 2996.774565656934,
            "unit": "ns/iter",
            "extra": "iterations: 233456\ncpu: 2996.7523045027747 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3087.1873205076763,
            "unit": "ns/iter",
            "extra": "iterations: 226681\ncpu: 3087.1936642241767 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 4103.995451858132,
            "unit": "ns/iter",
            "extra": "iterations: 170839\ncpu: 4103.53515883375 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.5588607104677932,
            "unit": "ns/iter",
            "extra": "iterations: 441577655\ncpu: 1.558814247518931 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 97.40342319444453,
            "unit": "us/iter",
            "extra": "iterations: 7200\ncpu: 97.40332874999991 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.234964764637642,
            "unit": "us/iter",
            "extra": "iterations: 36242\ncpu: 19.232234120633514 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 49.244796304154406,
            "unit": "us/iter",
            "extra": "iterations: 47134\ncpu: 15.258938218695613 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 49.419609829782296,
            "unit": "us/iter",
            "extra": "iterations: 44355\ncpu: 15.434958899785807 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 50.21394878281517,
            "unit": "us/iter",
            "extra": "iterations: 45063\ncpu: 15.784697357033474 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 58.91118648712482,
            "unit": "us/iter",
            "extra": "iterations: 42330\ncpu: 16.85861067800613 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 49.89349741266357,
            "unit": "us/iter",
            "extra": "iterations: 45027\ncpu: 15.066501210384867 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 493.6255362694297,
            "unit": "us/iter",
            "extra": "iterations: 1544\ncpu: 448.24353886010374 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 49.91727753533063,
            "unit": "us/iter",
            "extra": "iterations: 44225\ncpu: 15.853354482758633 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 99.90496616961747,
            "unit": "us/iter",
            "extra": "iterations: 21460\ncpu: 30.644710391425914 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 201.97024074742552,
            "unit": "us/iter",
            "extra": "iterations: 11078\ncpu: 62.66837190828682 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 403.06311122066415,
            "unit": "us/iter",
            "extra": "iterations: 5071\ncpu: 122.37225596529332 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.2976274256765703,
            "unit": "us/iter",
            "extra": "iterations: 562575\ncpu: 1.297558718393102 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.2402116553841993,
            "unit": "us/iter",
            "extra": "iterations: 565627\ncpu: 1.2401692581860486 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.29054209567265,
            "unit": "us/iter",
            "extra": "iterations: 540008\ncpu: 1.2904837761662762 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 2.0454133833646,
            "unit": "us/iter",
            "extra": "iterations: 337389\ncpu: 2.0453470326536958 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.238407801659171,
            "unit": "us/iter",
            "extra": "iterations: 559804\ncpu: 1.2383975516430774 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 4.3378172982178915,
            "unit": "us/iter",
            "extra": "iterations: 164410\ncpu: 4.337524451067468 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 10.530743463755675,
            "unit": "us/iter",
            "extra": "iterations: 71639\ncpu: 9.799098158824076 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 30.75120389636512,
            "unit": "us/iter",
            "extra": "iterations: 24895\ncpu: 28.828561156858825 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 66.16733336398798,
            "unit": "us/iter",
            "extra": "iterations: 10874\ncpu: 61.24085221629587 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.2507260615204943,
            "unit": "us/iter",
            "extra": "iterations: 562024\ncpu: 1.2506411007359137 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 50.334421065238324,
            "unit": "us/iter",
            "extra": "iterations: 45924\ncpu: 15.740045640623624 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 49.74293529690342,
            "unit": "us/iter",
            "extra": "iterations: 44341\ncpu: 14.91210859024379 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 59.31370953132052,
            "unit": "us/iter",
            "extra": "iterations: 44380\ncpu: 17.082265209553874 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 84.82798170684266,
            "unit": "us/iter",
            "extra": "iterations: 38539\ncpu: 18.369894859752545 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 20.28864587281735,
            "unit": "us/iter",
            "extra": "iterations: 74397\ncpu: 9.755239781174053 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 22.132092317704526,
            "unit": "us/iter",
            "extra": "iterations: 64538\ncpu: 10.947380179119302 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 25.397645899980574,
            "unit": "us/iter",
            "extra": "iterations: 61268\ncpu: 13.968189527975412 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 36.318361230063644,
            "unit": "us/iter",
            "extra": "iterations: 29657\ncpu: 23.381990659878014 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 29.854539312092125,
            "unit": "us/iter",
            "extra": "iterations: 46576\ncpu: 14.444021856750119 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 29.216956185326826,
            "unit": "us/iter",
            "extra": "iterations: 53635\ncpu: 13.816106684068236 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 31.680220807776916,
            "unit": "us/iter",
            "extra": "iterations: 43205\ncpu: 16.830730841337846 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 69.99134776364474,
            "unit": "us/iter",
            "extra": "iterations: 19496\ncpu: 38.38364546573652 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 60.723006290099704,
            "unit": "us/iter",
            "extra": "iterations: 23847\ncpu: 38.79267652115569 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 30.21978035071075,
            "unit": "us/iter",
            "extra": "iterations: 25263\ncpu: 20.870503107311094 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 56.31559519221917,
            "unit": "us/iter",
            "extra": "iterations: 21798\ncpu: 34.867332553445294 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 64.21969165179391,
            "unit": "us/iter",
            "extra": "iterations: 15129\ncpu: 47.78480904223675 us\nthreads: 1"
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
          "id": "2a7655aa927f91272c92eaac20f3daf3ce3e3792",
          "message": "Update Vortex submodule with cross-platform CI and test fixes (#56)",
          "timestamp": "2026-03-28T11:40:09+11:00",
          "tree_id": "0d965480ae5aaa8507328477eabaa3a443eaf51b",
          "url": "https://github.com/Mrunmoy/aether/commit/2a7655aa927f91272c92eaac20f3daf3ce3e3792"
        },
        "date": 1774658555462,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 41849.82380117011,
            "unit": "ns/iter",
            "extra": "iterations: 17100\ncpu: 41816.295380116964 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 41034.03613321531,
            "unit": "ns/iter",
            "extra": "iterations: 16965\ncpu: 40998.19823165341 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 41405.19585130918,
            "unit": "ns/iter",
            "extra": "iterations: 16921\ncpu: 41368.68500679628 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 41954.29321098754,
            "unit": "ns/iter",
            "extra": "iterations: 16674\ncpu: 41918.41303826317 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 43563.59985008426,
            "unit": "ns/iter",
            "extra": "iterations: 16009\ncpu: 43526.91080017487 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 3929.2505836532164,
            "unit": "ns/iter",
            "extra": "iterations: 178188\ncpu: 3929.117123487552 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 3952.0900187409216,
            "unit": "ns/iter",
            "extra": "iterations: 177152\ncpu: 3951.941728007584 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3978.2346142025935,
            "unit": "ns/iter",
            "extra": "iterations: 175714\ncpu: 3978.050178130375 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4535.865775685379,
            "unit": "ns/iter",
            "extra": "iterations: 154212\ncpu: 4535.415285451204 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 8006.597388330038,
            "unit": "ns/iter",
            "extra": "iterations: 88296\ncpu: 8006.178003533569 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 3929.963666062269,
            "unit": "ns/iter",
            "extra": "iterations: 178153\ncpu: 3929.670126239799 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 3948.490117551555,
            "unit": "ns/iter",
            "extra": "iterations: 177284\ncpu: 3948.427461022993 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3980.6228983413807,
            "unit": "ns/iter",
            "extra": "iterations: 175873\ncpu: 3980.3474609519376 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 4543.88572542907,
            "unit": "ns/iter",
            "extra": "iterations: 153840\ncpu: 4543.735153406144 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.2696738091038102,
            "unit": "ns/iter",
            "extra": "iterations: 552278994\ncpu: 1.2695594683436358 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 90.05990141576872,
            "unit": "us/iter",
            "extra": "iterations: 7699\ncpu: 90.05772892583425 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.959375744111348,
            "unit": "us/iter",
            "extra": "iterations: 35109\ncpu: 19.956998718277354 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 37.98171622688486,
            "unit": "us/iter",
            "extra": "iterations: 65866\ncpu: 9.153654814319992 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 38.21274482038185,
            "unit": "us/iter",
            "extra": "iterations: 86155\ncpu: 9.456211479310555 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 39.20702929241298,
            "unit": "us/iter",
            "extra": "iterations: 66536\ncpu: 9.682722150414843 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 41.66828582533965,
            "unit": "us/iter",
            "extra": "iterations: 68178\ncpu: 10.315560708733015 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 38.65482764999088,
            "unit": "us/iter",
            "extra": "iterations: 70821\ncpu: 8.782532186780797 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 350.39087271897785,
            "unit": "us/iter",
            "extra": "iterations: 2192\ncpu: 316.68655109489066 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 38.71411227952455,
            "unit": "us/iter",
            "extra": "iterations: 77503\ncpu: 9.677589164290398 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 77.29188502816187,
            "unit": "us/iter",
            "extra": "iterations: 35687\ncpu: 18.99025860957774 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 159.5313262687595,
            "unit": "us/iter",
            "extra": "iterations: 17458\ncpu: 39.77758425936528 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 322.350597057427,
            "unit": "us/iter",
            "extra": "iterations: 8428\ncpu: 78.14687968675844 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 0.9980933472405961,
            "unit": "us/iter",
            "extra": "iterations: 677824\ncpu: 0.9980544743178175 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 0.9351080904199858,
            "unit": "us/iter",
            "extra": "iterations: 747180\ncpu: 0.9350983993147556 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.01608968012574,
            "unit": "us/iter",
            "extra": "iterations: 688458\ncpu: 1.0160702947746996 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 0.021953414720734945,
            "unit": "us/iter",
            "extra": "iterations: 30063703\ncpu: 0.0219483248620438 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 0.9510655174898316,
            "unit": "us/iter",
            "extra": "iterations: 749464\ncpu: 0.9509520377229594 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 2.6602723393596226,
            "unit": "us/iter",
            "extra": "iterations: 231542\ncpu: 2.6600422342382775 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 8.65570383132742,
            "unit": "us/iter",
            "extra": "iterations: 80677\ncpu: 8.332949936165187 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 23.124929819365974,
            "unit": "us/iter",
            "extra": "iterations: 30393\ncpu: 22.271243016484114 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 47.62049039948425,
            "unit": "us/iter",
            "extra": "iterations: 15520\ncpu: 45.863315979381426 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 0.9524473238303912,
            "unit": "us/iter",
            "extra": "iterations: 721180\ncpu: 0.9524292298732611 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 38.41717070705639,
            "unit": "us/iter",
            "extra": "iterations: 70249\ncpu: 9.11509567395981 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 38.88745219816798,
            "unit": "us/iter",
            "extra": "iterations: 79589\ncpu: 8.979056477653947 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 41.52072085527648,
            "unit": "us/iter",
            "extra": "iterations: 69498\ncpu: 10.29286327664105 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 52.31356219543079,
            "unit": "us/iter",
            "extra": "iterations: 59305\ncpu: 11.917779698170532 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 14.802583912128632,
            "unit": "us/iter",
            "extra": "iterations: 88402\ncpu: 7.199395341734316 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 17.039986048668815,
            "unit": "us/iter",
            "extra": "iterations: 84078\ncpu: 8.139045517257731 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 14.349900457366862,
            "unit": "us/iter",
            "extra": "iterations: 91830\ncpu: 8.380175585320707 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 26.550584504121634,
            "unit": "us/iter",
            "extra": "iterations: 38578\ncpu: 17.636448182902193 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 20.77825530415552,
            "unit": "us/iter",
            "extra": "iterations: 105012\ncpu: 7.394571801317973 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 21.369637749588602,
            "unit": "us/iter",
            "extra": "iterations: 102719\ncpu: 7.660610237638609 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 24.107544449468392,
            "unit": "us/iter",
            "extra": "iterations: 68561\ncpu: 10.230493254182464 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 28.866662109052292,
            "unit": "us/iter",
            "extra": "iterations: 30279\ncpu: 21.835663264969163 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 33.646538607150845,
            "unit": "us/iter",
            "extra": "iterations: 42790\ncpu: 18.379651554101482 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 26.284495762248323,
            "unit": "us/iter",
            "extra": "iterations: 36576\ncpu: 15.699576252187176 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 34.43322438787682,
            "unit": "us/iter",
            "extra": "iterations: 35042\ncpu: 18.97403815421495 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 44.75403800317746,
            "unit": "us/iter",
            "extra": "iterations: 22656\ncpu: 30.90356024894066 us\nthreads: 1"
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
          "id": "3f266b8bd58d327e02c0530cea8ff3e185acac6c",
          "message": "Bump Vortex: behavioral contract tests",
          "timestamp": "2026-03-28T12:18:10+11:00",
          "tree_id": "8e814baf9d75390e23cac01b968344fe3c8847ed",
          "url": "https://github.com/Mrunmoy/aether/commit/3f266b8bd58d327e02c0530cea8ff3e185acac6c"
        },
        "date": 1774660820419,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 71629.51234754539,
            "unit": "ns/iter",
            "extra": "iterations: 9921\ncpu: 71565.0194536841 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 72819.61247285182,
            "unit": "ns/iter",
            "extra": "iterations: 9669\ncpu: 72762.15606577722 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 72026.30886720409,
            "unit": "ns/iter",
            "extra": "iterations: 9428\ncpu: 71974.11837081033 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 72779.8896452153,
            "unit": "ns/iter",
            "extra": "iterations: 9696\ncpu: 72726.47009075907 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 74545.28147829763,
            "unit": "ns/iter",
            "extra": "iterations: 9308\ncpu: 74487.25547915776 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2989.736953178419,
            "unit": "ns/iter",
            "extra": "iterations: 234080\ncpu: 2989.450410116201 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 3030.191917225741,
            "unit": "ns/iter",
            "extra": "iterations: 231183\ncpu: 3029.7110730460267 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3094.805898521395,
            "unit": "ns/iter",
            "extra": "iterations: 226294\ncpu: 3094.4791024066017 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 3943.637098989316,
            "unit": "ns/iter",
            "extra": "iterations: 178393\ncpu: 3943.3609222334935 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7037.328721843756,
            "unit": "ns/iter",
            "extra": "iterations: 99534\ncpu: 7036.41371792553 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2991.681321451113,
            "unit": "ns/iter",
            "extra": "iterations: 234076\ncpu: 2991.063731437654 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 3027.532304020529,
            "unit": "ns/iter",
            "extra": "iterations: 231287\ncpu: 3027.0901347676286 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3096.8499297312296,
            "unit": "ns/iter",
            "extra": "iterations: 226274\ncpu: 3096.593925948181 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 3920.4212889941323,
            "unit": "ns/iter",
            "extra": "iterations: 174353\ncpu: 3919.9596106748972 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.409099356654103,
            "unit": "ns/iter",
            "extra": "iterations: 496313110\ncpu: 1.40900919784287 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 95.28303502316683,
            "unit": "us/iter",
            "extra": "iterations: 7338\ncpu: 95.27328795312087 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.7154811171864,
            "unit": "us/iter",
            "extra": "iterations: 35482\ncpu: 19.712679894030764 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 40.858790827495504,
            "unit": "us/iter",
            "extra": "iterations: 54816\ncpu: 12.6488473803269 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 41.357466665455654,
            "unit": "us/iter",
            "extra": "iterations: 55048\ncpu: 13.024140259409975 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 42.08285756796733,
            "unit": "us/iter",
            "extra": "iterations: 56792\ncpu: 12.99556053669535 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 46.81300233896331,
            "unit": "us/iter",
            "extra": "iterations: 48312\ncpu: 14.513216571452233 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 41.506525573555734,
            "unit": "us/iter",
            "extra": "iterations: 53395\ncpu: 12.706850173237251 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 556.53712307692,
            "unit": "us/iter",
            "extra": "iterations: 1365\ncpu: 515.934063003664 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 41.466043863715164,
            "unit": "us/iter",
            "extra": "iterations: 51774\ncpu: 13.007300710781509 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 83.43264754980167,
            "unit": "us/iter",
            "extra": "iterations: 26957\ncpu: 25.171671959045845 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 167.4208843198952,
            "unit": "us/iter",
            "extra": "iterations: 13667\ncpu: 51.398325894490384 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 336.12415996478035,
            "unit": "us/iter",
            "extra": "iterations: 6814\ncpu: 104.26409113589665 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.3046330896244627,
            "unit": "us/iter",
            "extra": "iterations: 536706\ncpu: 1.304416498045485 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.31309474760154,
            "unit": "us/iter",
            "extra": "iterations: 532214\ncpu: 1.3128835693912544 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.4651191715613117,
            "unit": "us/iter",
            "extra": "iterations: 476028\ncpu: 1.464995338509498 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 3.1245317253329703,
            "unit": "us/iter",
            "extra": "iterations: 171393\ncpu: 3.124008215037945 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.3166401429378543,
            "unit": "us/iter",
            "extra": "iterations: 491962\ncpu: 1.3164421601668461 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 3.4500722262159194,
            "unit": "us/iter",
            "extra": "iterations: 223963\ncpu: 3.4497223737849567 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 13.62918582999569,
            "unit": "us/iter",
            "extra": "iterations: 56175\ncpu: 12.610577445482827 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 33.4944684515,
            "unit": "us/iter",
            "extra": "iterations: 22299\ncpu: 31.704284945513127 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 68.32709712531475,
            "unit": "us/iter",
            "extra": "iterations: 10749\ncpu: 64.32569094799506 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.3098649145708205,
            "unit": "us/iter",
            "extra": "iterations: 517095\ncpu: 1.3098161537048343 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 41.197442434756674,
            "unit": "us/iter",
            "extra": "iterations: 54182\ncpu: 12.70164945922997 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 41.46348825506229,
            "unit": "us/iter",
            "extra": "iterations: 58621\ncpu: 12.990496460312894 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 46.391834779008306,
            "unit": "us/iter",
            "extra": "iterations: 48305\ncpu: 14.459978904875284 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 60.870763598491564,
            "unit": "us/iter",
            "extra": "iterations: 37927\ncpu: 18.746567590370926 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 18.035395809182734,
            "unit": "us/iter",
            "extra": "iterations: 68149\ncpu: 10.776678615973823 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 19.139561938791726,
            "unit": "us/iter",
            "extra": "iterations: 64370\ncpu: 11.796620055926667 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 20.268554520358688,
            "unit": "us/iter",
            "extra": "iterations: 55062\ncpu: 12.887077730558241 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 36.93096873673838,
            "unit": "us/iter",
            "extra": "iterations: 28276\ncpu: 25.472482529353382 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 24.930407659700528,
            "unit": "us/iter",
            "extra": "iterations: 54023\ncpu: 11.79890794661538 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 25.41751162442701,
            "unit": "us/iter",
            "extra": "iterations: 53465\ncpu: 12.11345949686705 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 28.715374492022352,
            "unit": "us/iter",
            "extra": "iterations: 49953\ncpu: 16.061865793846255 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 37.880406473145065,
            "unit": "us/iter",
            "extra": "iterations: 25768\ncpu: 28.170356372244658 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 43.76628421709898,
            "unit": "us/iter",
            "extra": "iterations: 22423\ncpu: 27.38610297462434 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 33.520409638165006,
            "unit": "us/iter",
            "extra": "iterations: 30981\ncpu: 24.253765340046964 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 45.5812782409957,
            "unit": "us/iter",
            "extra": "iterations: 23627\ncpu: 29.935465992297043 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 77.65933100000098,
            "unit": "us/iter",
            "extra": "iterations: 10000\ncpu: 53.48167670000023 us\nthreads: 1"
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
          "id": "f55cb54d83cb85cc74181676a4946476c202b4b4",
          "message": "Bump Vortex: stress and concurrency tests",
          "timestamp": "2026-03-28T12:45:37+11:00",
          "tree_id": "7d9712b7008e4460a6878f4692ca6872a075c6cd",
          "url": "https://github.com/Mrunmoy/aether/commit/f55cb54d83cb85cc74181676a4946476c202b4b4"
        },
        "date": 1774662471257,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 61933.17417259231,
            "unit": "ns/iter",
            "extra": "iterations: 11391\ncpu: 61894.56237380388 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 61286.69485755339,
            "unit": "ns/iter",
            "extra": "iterations: 11162\ncpu: 61250.04094248343 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 61762.92983998584,
            "unit": "ns/iter",
            "extra": "iterations: 11374\ncpu: 61724.948918586226 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 62226.420590583395,
            "unit": "ns/iter",
            "extra": "iterations: 11277\ncpu: 62196.027134876276 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 63704.11129281333,
            "unit": "ns/iter",
            "extra": "iterations: 11007\ncpu: 63675.782229490345 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 3075.2906738625024,
            "unit": "ns/iter",
            "extra": "iterations: 229023\ncpu: 3075.0778262445265 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 3095.7017317090704,
            "unit": "ns/iter",
            "extra": "iterations: 225904\ncpu: 3095.5032580211055 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3151.883500605044,
            "unit": "ns/iter",
            "extra": "iterations: 222293\ncpu: 3151.7603433306504 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4228.454615567128,
            "unit": "ns/iter",
            "extra": "iterations: 168560\ncpu: 4228.106419079259 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7385.907665486824,
            "unit": "ns/iter",
            "extra": "iterations: 94645\ncpu: 7385.676496381217 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 3060.4451284913357,
            "unit": "ns/iter",
            "extra": "iterations: 229354\ncpu: 3060.16953268746 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 3099.561479782727,
            "unit": "ns/iter",
            "extra": "iterations: 225871\ncpu: 3099.3406501941386 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3144.6976118731563,
            "unit": "ns/iter",
            "extra": "iterations: 222350\ncpu: 3144.430159658196 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 4127.71999289566,
            "unit": "ns/iter",
            "extra": "iterations: 168910\ncpu: 4127.611805103312 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.5577276391964343,
            "unit": "ns/iter",
            "extra": "iterations: 449380485\ncpu: 1.557646418491004 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 97.42730393657094,
            "unit": "us/iter",
            "extra": "iterations: 7189\ncpu: 97.42262359159841 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.532052289586847,
            "unit": "us/iter",
            "extra": "iterations: 35858\ncpu: 19.531354481566197 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 48.31153291313535,
            "unit": "us/iter",
            "extra": "iterations: 46486\ncpu: 14.78090407864735 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 49.474881745329185,
            "unit": "us/iter",
            "extra": "iterations: 44049\ncpu: 15.046796658266938 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 49.5324808904563,
            "unit": "us/iter",
            "extra": "iterations: 45370\ncpu: 15.343383116596915 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 59.34848323163023,
            "unit": "us/iter",
            "extra": "iterations: 39658\ncpu: 16.099563896313565 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 48.760382172665636,
            "unit": "us/iter",
            "extra": "iterations: 46008\ncpu: 14.680598156842308 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 492.4965167741895,
            "unit": "us/iter",
            "extra": "iterations: 1550\ncpu: 447.5051632258056 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 49.418578888767634,
            "unit": "us/iter",
            "extra": "iterations: 45805\ncpu: 14.966142713677556 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 100.47834665720205,
            "unit": "us/iter",
            "extra": "iterations: 22541\ncpu: 31.344290093607125 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 199.51598454951525,
            "unit": "us/iter",
            "extra": "iterations: 10744\ncpu: 61.33775465376044 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 407.60386234092346,
            "unit": "us/iter",
            "extra": "iterations: 5579\ncpu: 124.84897651908918 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.2648762223279073,
            "unit": "us/iter",
            "extra": "iterations: 561531\ncpu: 1.264772144369585 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.2370774754837597,
            "unit": "us/iter",
            "extra": "iterations: 545863\ncpu: 1.2370244035591342 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.3289120704421564,
            "unit": "us/iter",
            "extra": "iterations: 535929\ncpu: 1.3288267513047405 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 0.04717576243411976,
            "unit": "us/iter",
            "extra": "iterations: 21450050\ncpu: 0.047172362768385157 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.2562882200372512,
            "unit": "us/iter",
            "extra": "iterations: 560596\ncpu: 1.2559432229270249 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 3.4847736066818906,
            "unit": "us/iter",
            "extra": "iterations: 164625\ncpu: 3.4822340774487515 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 10.561150367929015,
            "unit": "us/iter",
            "extra": "iterations: 70394\ncpu: 9.829168522885452 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 31.495543489635672,
            "unit": "us/iter",
            "extra": "iterations: 24845\ncpu: 29.41015524250343 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 66.53119672509274,
            "unit": "us/iter",
            "extra": "iterations: 13008\ncpu: 61.484160285977666 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.2475249769614916,
            "unit": "us/iter",
            "extra": "iterations: 561017\ncpu: 1.2474357996281769 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 51.32792034405381,
            "unit": "us/iter",
            "extra": "iterations: 42784\ncpu: 16.399089963537893 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 50.59168970702481,
            "unit": "us/iter",
            "extra": "iterations: 39901\ncpu: 15.36062665096118 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 60.59563191897255,
            "unit": "us/iter",
            "extra": "iterations: 42257\ncpu: 17.236329010578135 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 89.2310843401757,
            "unit": "us/iter",
            "extra": "iterations: 34100\ncpu: 20.812703079179016 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 21.233755681215495,
            "unit": "us/iter",
            "extra": "iterations: 84885\ncpu: 10.382808976850985 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 17.970818990276427,
            "unit": "us/iter",
            "extra": "iterations: 57798\ncpu: 9.01857676736215 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 26.6503878576056,
            "unit": "us/iter",
            "extra": "iterations: 43204\ncpu: 15.160517822423863 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 44.62397757971487,
            "unit": "us/iter",
            "extra": "iterations: 28947\ncpu: 28.29314875462065 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 30.69715874338606,
            "unit": "us/iter",
            "extra": "iterations: 48002\ncpu: 15.078937481771469 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 31.1994662875983,
            "unit": "us/iter",
            "extra": "iterations: 59444\ncpu: 15.488628524325508 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 33.992274591847334,
            "unit": "us/iter",
            "extra": "iterations: 38956\ncpu: 18.987722558784196 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 68.28885721506141,
            "unit": "us/iter",
            "extra": "iterations: 19785\ncpu: 40.10260293151363 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 50.72383993399352,
            "unit": "us/iter",
            "extra": "iterations: 25452\ncpu: 30.07538189533252 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 53.9993841713553,
            "unit": "us/iter",
            "extra": "iterations: 27545\ncpu: 33.01247874387373 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 55.13674353826839,
            "unit": "us/iter",
            "extra": "iterations: 23910\ncpu: 34.308625554161424 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 64.28910304914238,
            "unit": "us/iter",
            "extra": "iterations: 15119\ncpu: 47.3955512930747 us\nthreads: 1"
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
          "id": "6740cc42cf97523e310a1f2bbcfafec6d18d43ec",
          "message": "Update Vortex submodule to include IOCP backend (#59)",
          "timestamp": "2026-03-28T14:54:22+11:00",
          "tree_id": "c2a5ae96cddd738dd02c919469aa8053894d3618",
          "url": "https://github.com/Mrunmoy/aether/commit/6740cc42cf97523e310a1f2bbcfafec6d18d43ec"
        },
        "date": 1774670203573,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 61390.58315088864,
            "unit": "ns/iter",
            "extra": "iterations: 11419\ncpu: 61346.289167177514 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 61543.54125296464,
            "unit": "ns/iter",
            "extra": "iterations: 11381\ncpu: 61511.85423073544 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 61511.68897499043,
            "unit": "ns/iter",
            "extra": "iterations: 11356\ncpu: 61477.94725255371 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 62258.8310135617,
            "unit": "ns/iter",
            "extra": "iterations: 11208\ncpu: 62226.868219129225 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 63654.28748405978,
            "unit": "ns/iter",
            "extra": "iterations: 10978\ncpu: 63628.65722353799 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 3046.1342351217886,
            "unit": "ns/iter",
            "extra": "iterations: 229396\ncpu: 3045.814883433016 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 3129.358409927082,
            "unit": "ns/iter",
            "extra": "iterations: 225323\ncpu: 3129.017903187866 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3142.497598942474,
            "unit": "ns/iter",
            "extra": "iterations: 222402\ncpu: 3142.009559266554 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4176.661643384918,
            "unit": "ns/iter",
            "extra": "iterations: 167155\ncpu: 4176.3779665579805 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7445.317329083652,
            "unit": "ns/iter",
            "extra": "iterations: 93496\ncpu: 7444.368550526226 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 3038.8219922933467,
            "unit": "ns/iter",
            "extra": "iterations: 229153\ncpu: 3038.7081993253405 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 3120.614037184114,
            "unit": "ns/iter",
            "extra": "iterations: 225957\ncpu: 3120.237315064372 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3139.358842362974,
            "unit": "ns/iter",
            "extra": "iterations: 223006\ncpu: 3139.2364241320856 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 4186.50741907006,
            "unit": "ns/iter",
            "extra": "iterations: 167676\ncpu: 4186.332486461985 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.5603689288308786,
            "unit": "ns/iter",
            "extra": "iterations: 448698031\ncpu: 1.5603082733385123 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 97.49680875993894,
            "unit": "us/iter",
            "extra": "iterations: 7169\ncpu: 97.48890291532972 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.922153159475567,
            "unit": "us/iter",
            "extra": "iterations: 35623\ncpu: 19.921938017572913 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 45.48500937523711,
            "unit": "us/iter",
            "extra": "iterations: 52692\ncpu: 13.761905089956734 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 44.7672498245035,
            "unit": "us/iter",
            "extra": "iterations: 48434\ncpu: 13.320430276252235 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 46.96794686910915,
            "unit": "us/iter",
            "extra": "iterations: 48804\ncpu: 14.366692812064578 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 55.16892531801377,
            "unit": "us/iter",
            "extra": "iterations: 46303\ncpu: 14.894848562728166 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 45.21348273545385,
            "unit": "us/iter",
            "extra": "iterations: 56561\ncpu: 13.835664521490024 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 486.3058675159215,
            "unit": "us/iter",
            "extra": "iterations: 1570\ncpu: 443.6819299363055 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 46.011716514384666,
            "unit": "us/iter",
            "extra": "iterations: 45639\ncpu: 13.999400534630471 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 92.62887861338251,
            "unit": "us/iter",
            "extra": "iterations: 26049\ncpu: 28.314844331836177 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 187.6155321641961,
            "unit": "us/iter",
            "extra": "iterations: 12887\ncpu: 56.733096220998036 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 382.4418503286956,
            "unit": "us/iter",
            "extra": "iterations: 6541\ncpu: 113.30063751719894 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.2754325845195524,
            "unit": "us/iter",
            "extra": "iterations: 562089\ncpu: 1.2752701814125524 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.2344842392978523,
            "unit": "us/iter",
            "extra": "iterations: 564886\ncpu: 1.2344449340220844 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.2854312125911809,
            "unit": "us/iter",
            "extra": "iterations: 535803\ncpu: 1.2853679524004125 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 2.0182310919278565,
            "unit": "us/iter",
            "extra": "iterations: 717987\ncpu: 2.01803318862319 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.2545748128258727,
            "unit": "us/iter",
            "extra": "iterations: 563112\ncpu: 1.254544692707666 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 3.161091873669556,
            "unit": "us/iter",
            "extra": "iterations: 241462\ncpu: 3.160839875425539 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 10.617534051724137,
            "unit": "us/iter",
            "extra": "iterations: 71920\ncpu: 9.707369785873167 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 29.72857443346898,
            "unit": "us/iter",
            "extra": "iterations: 25815\ncpu: 27.80351667635096 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 64.77278032954173,
            "unit": "us/iter",
            "extra": "iterations: 13716\ncpu: 59.93826122776317 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.2542020382347305,
            "unit": "us/iter",
            "extra": "iterations: 561270\ncpu: 1.2541509932830952 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 46.161445925636976,
            "unit": "us/iter",
            "extra": "iterations: 51235\ncpu: 13.846206460427481 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 45.17736873923881,
            "unit": "us/iter",
            "extra": "iterations: 52270\ncpu: 13.73640208532622 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 54.999614950872285,
            "unit": "us/iter",
            "extra": "iterations: 47529\ncpu: 14.34488316606696 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 82.59244549107936,
            "unit": "us/iter",
            "extra": "iterations: 39067\ncpu: 18.50505956433821 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 19.079858492870397,
            "unit": "us/iter",
            "extra": "iterations: 77777\ncpu: 9.260684598274528 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 17.29300770196307,
            "unit": "us/iter",
            "extra": "iterations: 82057\ncpu: 8.732907198654534 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 21.362546653619464,
            "unit": "us/iter",
            "extra": "iterations: 49053\ncpu: 11.984073614254028 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 42.56434668425807,
            "unit": "us/iter",
            "extra": "iterations: 31833\ncpu: 27.415910281782924 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 26.506801546116815,
            "unit": "us/iter",
            "extra": "iterations: 56270\ncpu: 12.120472418695556 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 26.966758859118485,
            "unit": "us/iter",
            "extra": "iterations: 54379\ncpu: 12.561303922470131 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 29.784039437371955,
            "unit": "us/iter",
            "extra": "iterations: 44932\ncpu: 16.062792953796748 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 66.64321020378402,
            "unit": "us/iter",
            "extra": "iterations: 20561\ncpu: 38.40141423082531 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 49.83344970459548,
            "unit": "us/iter",
            "extra": "iterations: 26066\ncpu: 30.7601321645055 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 31.210264526282597,
            "unit": "us/iter",
            "extra": "iterations: 31770\ncpu: 21.736812747875376 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 53.12319990523779,
            "unit": "us/iter",
            "extra": "iterations: 23216\ncpu: 33.838139386629955 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 61.02881317122612,
            "unit": "us/iter",
            "extra": "iterations: 14805\ncpu: 44.40161526511336 us\nthreads: 1"
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
          "id": "4761d570972f8d4eae805e68acdd9970fe348496",
          "message": "Bump Vortex and Ouroboros submodules (#60)\n\nVortex: IOCP default backend, README docs (PRs #16-17)\nOuroboros: Windows + macOS CI expansion (PR #8)\nBacklog: Phase 5c IOCP items marked done",
          "timestamp": "2026-03-28T15:47:50+11:00",
          "tree_id": "e244713aa1c097853e07a1bfa6593df31d78980d",
          "url": "https://github.com/Mrunmoy/aether/commit/4761d570972f8d4eae805e68acdd9970fe348496"
        },
        "date": 1774673412710,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 63209.7969642853,
            "unit": "ns/iter",
            "extra": "iterations: 11200\ncpu: 63072.87062500001 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 62847.888115631584,
            "unit": "ns/iter",
            "extra": "iterations: 11208\ncpu: 62712.32762312634 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 62893.78038934137,
            "unit": "ns/iter",
            "extra": "iterations: 11147\ncpu: 62761.779581950286 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 63335.13038754059,
            "unit": "ns/iter",
            "extra": "iterations: 11044\ncpu: 63220.954545454544 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 64104.46439345531,
            "unit": "ns/iter",
            "extra": "iterations: 10939\ncpu: 64053.56623091689 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2949.7471499995645,
            "unit": "ns/iter",
            "extra": "iterations: 235614\ncpu: 2949.490756067124 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 2976.9932985916084,
            "unit": "ns/iter",
            "extra": "iterations: 234876\ncpu: 2976.5274570411625 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3045.6874187390686,
            "unit": "ns/iter",
            "extra": "iterations: 229969\ncpu: 3045.0472150594214 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4326.1212646060385,
            "unit": "ns/iter",
            "extra": "iterations: 161663\ncpu: 4325.652907591725 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7838.73573583678,
            "unit": "ns/iter",
            "extra": "iterations: 89157\ncpu: 7837.967932972159 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2997.0941539938312,
            "unit": "ns/iter",
            "extra": "iterations: 233373\ncpu: 2996.5773332819163 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 3050.9168258959385,
            "unit": "ns/iter",
            "extra": "iterations: 230276\ncpu: 3050.623195643488 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3096.239100492704,
            "unit": "ns/iter",
            "extra": "iterations: 226524\ncpu: 3095.7585907012085 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 4371.985783409023,
            "unit": "ns/iter",
            "extra": "iterations: 160376\ncpu: 4371.726424153247 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.5600386786469176,
            "unit": "ns/iter",
            "extra": "iterations: 446339295\ncpu: 1.5599462198370861 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 98.52955309920239,
            "unit": "us/iter",
            "extra": "iterations: 7147\ncpu: 98.52090499510277 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.027690044697316,
            "unit": "us/iter",
            "extra": "iterations: 36915\ncpu: 19.02555015576321 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 50.441538906675866,
            "unit": "us/iter",
            "extra": "iterations: 42165\ncpu: 16.06144347207393 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 50.94560472760327,
            "unit": "us/iter",
            "extra": "iterations: 42897\ncpu: 15.893986572487659 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 51.25562534073226,
            "unit": "us/iter",
            "extra": "iterations: 41088\ncpu: 16.21328300233647 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 60.39187572491678,
            "unit": "us/iter",
            "extra": "iterations: 38970\ncpu: 16.28556176546066 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 51.386546184357414,
            "unit": "us/iter",
            "extra": "iterations: 42103\ncpu: 16.187751181625984 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 498.72917654723744,
            "unit": "us/iter",
            "extra": "iterations: 1535\ncpu: 452.9698254071663 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 50.37437966922955,
            "unit": "us/iter",
            "extra": "iterations: 40451\ncpu: 15.52655358334775 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 102.12595966228889,
            "unit": "us/iter",
            "extra": "iterations: 23452\ncpu: 32.102178279038064 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 206.5504605365473,
            "unit": "us/iter",
            "extra": "iterations: 10288\ncpu: 63.783928071539776 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 417.0598784703065,
            "unit": "us/iter",
            "extra": "iterations: 5439\ncpu: 128.09190568119124 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.2528768959945125,
            "unit": "us/iter",
            "extra": "iterations: 553865\ncpu: 1.2528094734276434 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.2401996719379675,
            "unit": "us/iter",
            "extra": "iterations: 558431\ncpu: 1.2400525454353362 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.2924066842904682,
            "unit": "us/iter",
            "extra": "iterations: 538666\ncpu: 1.2922560566287826 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 0.2483338273510833,
            "unit": "us/iter",
            "extra": "iterations: 14095310\ncpu: 0.24830580774739952 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.2403053125317334,
            "unit": "us/iter",
            "extra": "iterations: 531498\ncpu: 1.2401749696141922 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 3.5402918987546252,
            "unit": "us/iter",
            "extra": "iterations: 160679\ncpu: 3.5398055750907207 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 10.59133449038935,
            "unit": "us/iter",
            "extra": "iterations: 69141\ncpu: 9.784649657945346 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 31.20123517641213,
            "unit": "us/iter",
            "extra": "iterations: 24488\ncpu: 29.237554516497706 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 68.66907039895678,
            "unit": "us/iter",
            "extra": "iterations: 10753\ncpu: 63.805211382870304 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.2393208335027392,
            "unit": "us/iter",
            "extra": "iterations: 565661\ncpu: 1.2392174730094487 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 50.232602364307674,
            "unit": "us/iter",
            "extra": "iterations: 43057\ncpu: 15.2698313166267 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 51.37040723307077,
            "unit": "us/iter",
            "extra": "iterations: 42914\ncpu: 15.077080603066596 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 61.09831956728788,
            "unit": "us/iter",
            "extra": "iterations: 44094\ncpu: 16.456665895586607 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 86.82781365848565,
            "unit": "us/iter",
            "extra": "iterations: 38628\ncpu: 19.61248526975254 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 18.72559884882479,
            "unit": "us/iter",
            "extra": "iterations: 84088\ncpu: 9.1727400104652 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 20.317484147025628,
            "unit": "us/iter",
            "extra": "iterations: 85536\ncpu: 9.918393693883285 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 25.92512127026239,
            "unit": "us/iter",
            "extra": "iterations: 55768\ncpu: 14.598257818103523 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 45.61967050646685,
            "unit": "us/iter",
            "extra": "iterations: 31078\ncpu: 28.775358227685082 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 32.27054436306893,
            "unit": "us/iter",
            "extra": "iterations: 49974\ncpu: 15.978877536318972 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 28.197110381838645,
            "unit": "us/iter",
            "extra": "iterations: 51802\ncpu: 12.63969972201846 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 32.95993186368874,
            "unit": "us/iter",
            "extra": "iterations: 51940\ncpu: 17.413558721601827 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 65.58168933250202,
            "unit": "us/iter",
            "extra": "iterations: 17633\ncpu: 36.45060970906841 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 51.775527258336226,
            "unit": "us/iter",
            "extra": "iterations: 20214\ncpu: 30.91981443553951 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 30.611533974895224,
            "unit": "us/iter",
            "extra": "iterations: 23900\ncpu: 21.01552891213395 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 53.863275193020826,
            "unit": "us/iter",
            "extra": "iterations: 19946\ncpu: 32.76931881078918 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 64.8563098462257,
            "unit": "us/iter",
            "extra": "iterations: 13071\ncpu: 48.16588264096088 us\nthreads: 1"
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
          "id": "5bb508bdd98805bc069fb30dc1a0361ef2bb741e",
          "message": "Enable Windows RunLoop mode with async named pipe I/O (#61)\n\n* Enable Windows RunLoop mode with async named pipe I/O\n\n* Remove RunLoop Windows guards from codegen example tests\n\n* Fix review findings: event handle leak, m_running guard, settle timing\n\n- Close event handles eagerly in disconnect/stop instead of deferring to\n  destructors, preventing leak on reconnect/restart cycles\n- Add m_running guard to onAcceptReady, consistent with onClientReady\n- Increase settle() to 50ms on Windows for named pipe handshake timing\n- Document lock ordering for async I/O mutexes\n\n* Fix Round 2 findings: deferred event close, accept handler mutex\n\n- Use executeOnRunLoop to defer CloseHandle after IOCP backend retires\n  the threadpool wait (prevents UB from closing while TP wait registered)\n- Close event handle in onDataReady error path (prevents leak on\n  reconnect after server-side disconnect)\n- Add m_acceptHandlerMutex to serialize onAcceptReady vs stop() on\n  Windows (prevents TOCTOU race on non-atomic m_listenFd)",
          "timestamp": "2026-03-28T16:41:52+11:00",
          "tree_id": "67249e348689d5ac93fa50623acde5e6c0d8d5d3",
          "url": "https://github.com/Mrunmoy/aether/commit/5bb508bdd98805bc069fb30dc1a0361ef2bb741e"
        },
        "date": 1774676649992,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 61770.30423608682,
            "unit": "ns/iter",
            "extra": "iterations: 11284\ncpu: 61729.93078695498 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 61701.12959843898,
            "unit": "ns/iter",
            "extra": "iterations: 11281\ncpu: 61659.43542239162 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 61712.46183406036,
            "unit": "ns/iter",
            "extra": "iterations: 11450\ncpu: 61663.19441048034 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 61438.16144131515,
            "unit": "ns/iter",
            "extra": "iterations: 11323\ncpu: 61392.854013953875 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 62901.04761904724,
            "unit": "ns/iter",
            "extra": "iterations: 11088\ncpu: 62851.0208333333 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2946.7246802794,
            "unit": "ns/iter",
            "extra": "iterations: 237473\ncpu: 2946.1115495235244 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 2991.1188797442915,
            "unit": "ns/iter",
            "extra": "iterations: 234018\ncpu: 2990.583297011343 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3045.136911138442,
            "unit": "ns/iter",
            "extra": "iterations: 229638\ncpu: 3044.8840305175972 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4001.659432137429,
            "unit": "ns/iter",
            "extra": "iterations: 173704\ncpu: 4001.3430490950086 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 6968.0404409932935,
            "unit": "ns/iter",
            "extra": "iterations: 100319\ncpu: 6967.115441740851 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2941.963132632515,
            "unit": "ns/iter",
            "extra": "iterations: 237717\ncpu: 2941.515958050957 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 2984.812888539033,
            "unit": "ns/iter",
            "extra": "iterations: 235015\ncpu: 2984.6611322681474 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3033.7827342997457,
            "unit": "ns/iter",
            "extra": "iterations: 230538\ncpu: 3033.5555439884133 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 4005.9041323165284,
            "unit": "ns/iter",
            "extra": "iterations: 174793\ncpu: 4005.6113288289557 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.353448876570626,
            "unit": "ns/iter",
            "extra": "iterations: 516903929\ncpu: 1.3533373548801164 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 97.09650277469392,
            "unit": "us/iter",
            "extra": "iterations: 7208\ncpu: 97.0922321032188 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.104660355739814,
            "unit": "us/iter",
            "extra": "iterations: 36656\ncpu: 19.102560535792232 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 48.65315217015006,
            "unit": "us/iter",
            "extra": "iterations: 46218\ncpu: 14.990378402354096 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 49.52430231957652,
            "unit": "us/iter",
            "extra": "iterations: 46474\ncpu: 14.974146942376425 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 50.530386031655524,
            "unit": "us/iter",
            "extra": "iterations: 49154\ncpu: 15.75702738332582 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 58.58310996767778,
            "unit": "us/iter",
            "extra": "iterations: 42076\ncpu: 15.993638820230105 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 49.61269251667582,
            "unit": "us/iter",
            "extra": "iterations: 49323\ncpu: 15.192063702532312 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 494.62873932729605,
            "unit": "us/iter",
            "extra": "iterations: 1546\ncpu: 448.7804540750311 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 50.97283281563661,
            "unit": "us/iter",
            "extra": "iterations: 47325\ncpu: 16.205118478605414 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 101.7873222156544,
            "unit": "us/iter",
            "extra": "iterations: 23686\ncpu: 32.28468985898854 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 205.97503977591114,
            "unit": "us/iter",
            "extra": "iterations: 10710\ncpu: 63.67342446311877 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 411.91790458579777,
            "unit": "us/iter",
            "extra": "iterations: 5408\ncpu: 128.20777496301767 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.3017512207510682,
            "unit": "us/iter",
            "extra": "iterations: 555191\ncpu: 1.3014333175429735 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.2559180935124559,
            "unit": "us/iter",
            "extra": "iterations: 559626\ncpu: 1.2558030559695232 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.3065539215047264,
            "unit": "us/iter",
            "extra": "iterations: 536975\ncpu: 1.3064751748219148 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 1.986274613349067,
            "unit": "us/iter",
            "extra": "iterations: 345402\ncpu: 1.9860514212424947 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.2553830334976313,
            "unit": "us/iter",
            "extra": "iterations: 555860\ncpu: 1.2553200104342837 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 4.1278746235726365,
            "unit": "us/iter",
            "extra": "iterations: 258350\ncpu: 4.127582752080515 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 10.396770918911324,
            "unit": "us/iter",
            "extra": "iterations: 70355\ncpu: 9.671308236799058 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 31.396953543209502,
            "unit": "us/iter",
            "extra": "iterations: 24173\ncpu: 29.365199065072655 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 68.1762465353218,
            "unit": "us/iter",
            "extra": "iterations: 13349\ncpu: 62.98842535021326 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.2494608053845087,
            "unit": "us/iter",
            "extra": "iterations: 558342\ncpu: 1.2494373573902757 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 51.55797841696169,
            "unit": "us/iter",
            "extra": "iterations: 47213\ncpu: 16.490369559231524 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 50.28190877010446,
            "unit": "us/iter",
            "extra": "iterations: 44207\ncpu: 15.7247939919017 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 59.300636414074226,
            "unit": "us/iter",
            "extra": "iterations: 43258\ncpu: 16.935468075269217 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 89.56777792154355,
            "unit": "us/iter",
            "extra": "iterations: 34006\ncpu: 19.48485340822199 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 18.59303784574229,
            "unit": "us/iter",
            "extra": "iterations: 80009\ncpu: 9.299531990151117 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 22.079444927754682,
            "unit": "us/iter",
            "extra": "iterations: 66440\ncpu: 10.99363395544851 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 21.72119809941533,
            "unit": "us/iter",
            "extra": "iterations: 46512\ncpu: 12.034266640866905 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 37.85869017169493,
            "unit": "us/iter",
            "extra": "iterations: 28306\ncpu: 24.328849607856995 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 29.6939005589989,
            "unit": "us/iter",
            "extra": "iterations: 55993\ncpu: 14.1907010161984 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 28.86044484684425,
            "unit": "us/iter",
            "extra": "iterations: 59090\ncpu: 13.343184464376293 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 32.13582304475818,
            "unit": "us/iter",
            "extra": "iterations: 40417\ncpu: 16.97167033673947 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 67.85110677498105,
            "unit": "us/iter",
            "extra": "iterations: 18834\ncpu: 38.773574015079134 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 56.39395276678803,
            "unit": "us/iter",
            "extra": "iterations: 20981\ncpu: 34.86222515609344 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 34.79356371497262,
            "unit": "us/iter",
            "extra": "iterations: 30762\ncpu: 23.845808139912883 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 59.75258238789726,
            "unit": "us/iter",
            "extra": "iterations: 22212\ncpu: 37.66805884206725 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 63.975249968814865,
            "unit": "us/iter",
            "extra": "iterations: 16034\ncpu: 48.00273138331077 us\nthreads: 1"
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
          "id": "e8ba1039eb4485e09fb465f559946dfbdfb24cb7",
          "message": "Fix stack OVERLAPPED use-after-cancel on Windows (#62)\n\nwaitForOverlapped and sendSignal called CancelIoEx on stack-allocated\nOVERLAPPED structures but returned immediately, allowing the kernel to\nreference freed stack memory when the cancellation completed.\n\nAdd GetOverlappedResult(TRUE) after every CancelIoEx that guards a\nstack or soon-to-be-freed OVERLAPPED. This blocks until the kernel\nsignals ERROR_OPERATION_ABORTED, which happens almost immediately for\nnamed pipe I/O. Also remove a redundant CancelIoEx in sendSignal where\nthe I/O had already completed, and add clarifying comments to\ncancelAccept and cancelRecvSignal.",
          "timestamp": "2026-03-28T17:05:14+11:00",
          "tree_id": "5b7a5500a0f11076cd93f759c7870214b1070058",
          "url": "https://github.com/Mrunmoy/aether/commit/e8ba1039eb4485e09fb465f559946dfbdfb24cb7"
        },
        "date": 1774678048140,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 62302.010587708006,
            "unit": "ns/iter",
            "extra": "iterations: 11145\ncpu: 62198.44262000898 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 62346.916244782406,
            "unit": "ns/iter",
            "extra": "iterations: 11259\ncpu: 62249.23598898659 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 62930.0481723128,
            "unit": "ns/iter",
            "extra": "iterations: 11189\ncpu: 62838.92859058004 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 63640.8478593965,
            "unit": "ns/iter",
            "extra": "iterations: 11095\ncpu: 63544.86894997745 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 64552.84263679845,
            "unit": "ns/iter",
            "extra": "iterations: 10892\ncpu: 64513.1530481087 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2945.300512181322,
            "unit": "ns/iter",
            "extra": "iterations: 235854\ncpu: 2944.6361859455424 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 3000.1130890634167,
            "unit": "ns/iter",
            "extra": "iterations: 234249\ncpu: 2999.300773962749 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3049.488134752596,
            "unit": "ns/iter",
            "extra": "iterations: 229873\ncpu: 3049.152010022924 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 3929.2339811721135,
            "unit": "ns/iter",
            "extra": "iterations: 177822\ncpu: 3928.7547772491585 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7180.494376944902,
            "unit": "ns/iter",
            "extra": "iterations: 97367\ncpu: 7179.883944252166 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2945.9132485470573,
            "unit": "ns/iter",
            "extra": "iterations: 237460\ncpu: 2945.708982565491 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 2982.0801457352277,
            "unit": "ns/iter",
            "extra": "iterations: 231653\ncpu: 2981.6532442921116 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3042.54398486431,
            "unit": "ns/iter",
            "extra": "iterations: 229920\ncpu: 3042.355497564367 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 3923.8897519735747,
            "unit": "ns/iter",
            "extra": "iterations: 173409\ncpu: 3923.5381266254894 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.3549235389777177,
            "unit": "ns/iter",
            "extra": "iterations: 517391984\ncpu: 1.3548264269204446 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 101.70367194755804,
            "unit": "us/iter",
            "extra": "iterations: 7322\ncpu: 101.69505053264133 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.068332071772684,
            "unit": "us/iter",
            "extra": "iterations: 36727\ncpu: 19.06686987774662 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 47.36127746966746,
            "unit": "us/iter",
            "extra": "iterations: 47061\ncpu: 14.711187735067238 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 48.64122962250048,
            "unit": "us/iter",
            "extra": "iterations: 50914\ncpu: 14.82481486428091 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 49.44772510135876,
            "unit": "us/iter",
            "extra": "iterations: 47603\ncpu: 14.771639119383233 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 57.140707651635346,
            "unit": "us/iter",
            "extra": "iterations: 46147\ncpu: 15.514412876243313 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 48.50531544361479,
            "unit": "us/iter",
            "extra": "iterations: 49153\ncpu: 14.837889630337886 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 491.6753101265795,
            "unit": "us/iter",
            "extra": "iterations: 1580\ncpu: 448.13040632911355 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 47.9837728268257,
            "unit": "us/iter",
            "extra": "iterations: 52964\ncpu: 14.54457197341592 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 97.6791880560124,
            "unit": "us/iter",
            "extra": "iterations: 24280\ncpu: 29.296050123558476 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 198.46265170694224,
            "unit": "us/iter",
            "extra": "iterations: 13035\ncpu: 60.8478133486767 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 389.4712207580941,
            "unit": "us/iter",
            "extra": "iterations: 6147\ncpu: 116.82361997722433 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.2579257765732212,
            "unit": "us/iter",
            "extra": "iterations: 560443\ncpu: 1.257812157525388 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.2504830229505037,
            "unit": "us/iter",
            "extra": "iterations: 562907\ncpu: 1.2503960370007796 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.3000812100219101,
            "unit": "us/iter",
            "extra": "iterations: 536387\ncpu: 1.2999945822698917 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 2.0346320109853644,
            "unit": "us/iter",
            "extra": "iterations: 329893\ncpu: 2.034589215291016 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.2674415379118593,
            "unit": "us/iter",
            "extra": "iterations: 554154\ncpu: 1.2673944824001997 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 4.261274146926984,
            "unit": "us/iter",
            "extra": "iterations: 163292\ncpu: 4.257639173995043 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 10.408540143003096,
            "unit": "us/iter",
            "extra": "iterations: 73425\ncpu: 9.624281988423537 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 30.490934170396415,
            "unit": "us/iter",
            "extra": "iterations: 25247\ncpu: 28.44099821760997 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 64.07783837471906,
            "unit": "us/iter",
            "extra": "iterations: 13290\ncpu: 59.261004740406726 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.2490508673684169,
            "unit": "us/iter",
            "extra": "iterations: 562679\ncpu: 1.2489034138469768 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 46.46451430205947,
            "unit": "us/iter",
            "extra": "iterations: 48944\ncpu: 14.086974930532897 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 47.41388849186799,
            "unit": "us/iter",
            "extra": "iterations: 48696\ncpu: 13.932384569574577 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 56.541179864064524,
            "unit": "us/iter",
            "extra": "iterations: 47964\ncpu: 15.083552143274149 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 85.11418658833536,
            "unit": "us/iter",
            "extra": "iterations: 39831\ncpu: 18.415033617031995 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 18.816821555158615,
            "unit": "us/iter",
            "extra": "iterations: 72790\ncpu: 9.412031542794386 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 19.843998900005825,
            "unit": "us/iter",
            "extra": "iterations: 66364\ncpu: 9.79189010608161 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 20.84934305890779,
            "unit": "us/iter",
            "extra": "iterations: 61316\ncpu: 12.167049301976645 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 43.69413592456242,
            "unit": "us/iter",
            "extra": "iterations: 30436\ncpu: 27.992546589564995 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 26.785325453583244,
            "unit": "us/iter",
            "extra": "iterations: 58589\ncpu: 11.928196914096517 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 27.505318397352767,
            "unit": "us/iter",
            "extra": "iterations: 58628\ncpu: 12.674280053899164 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 29.82928990999505,
            "unit": "us/iter",
            "extra": "iterations: 48553\ncpu: 15.699977385537489 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 54.382201405265356,
            "unit": "us/iter",
            "extra": "iterations: 20779\ncpu: 31.498182106934983 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 45.236466829149485,
            "unit": "us/iter",
            "extra": "iterations: 20515\ncpu: 26.746436558615702 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 50.7218061551614,
            "unit": "us/iter",
            "extra": "iterations: 23395\ncpu: 31.32795734131213 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 46.823552852203925,
            "unit": "us/iter",
            "extra": "iterations: 23140\ncpu: 28.547618798616956 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 64.37644330943468,
            "unit": "us/iter",
            "extra": "iterations: 15761\ncpu: 47.10772882431291 us\nthreads: 1"
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
          "id": "4baece6ab12e80bc6a0793a4d4ab4c1c6983d690",
          "message": "Bump Vortex submodule (WFMO retired) and update docs (#63)\n\nBump deps/vortex to Vortex#18 which removes the legacy WFMO backend.\nUpdate aether-vision.md to reflect that IOCP and Windows RunLoop\nintegration are complete — no more 'will use' or 'current gaps'.",
          "timestamp": "2026-03-28T17:54:54+11:00",
          "tree_id": "54dd9626854d8192601d1ab5bda0a455c0b7fa42",
          "url": "https://github.com/Mrunmoy/aether/commit/4baece6ab12e80bc6a0793a4d4ab4c1c6983d690"
        },
        "date": 1774681029272,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 62263.21017069692,
            "unit": "ns/iter",
            "extra": "iterations: 11248\ncpu: 62212.58303698435 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 62358.40172001121,
            "unit": "ns/iter",
            "extra": "iterations: 11279\ncpu: 62299.3241422112 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 62128.913124667364,
            "unit": "ns/iter",
            "extra": "iterations: 11246\ncpu: 62074.24728792462 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 62941.37072380229,
            "unit": "ns/iter",
            "extra": "iterations: 11108\ncpu: 62891.93986316169 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 64337.319304412136,
            "unit": "ns/iter",
            "extra": "iterations: 10811\ncpu: 64303.38858569976 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2969.6339925431625,
            "unit": "ns/iter",
            "extra": "iterations: 236293\ncpu: 2969.2325291058155 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 3032.184574523323,
            "unit": "ns/iter",
            "extra": "iterations: 211974\ncpu: 3031.7702123845393 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3168.753035011015,
            "unit": "ns/iter",
            "extra": "iterations: 221416\ncpu: 3168.2734897207047 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4357.051358771103,
            "unit": "ns/iter",
            "extra": "iterations: 162095\ncpu: 4356.740818655731 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7590.957480929331,
            "unit": "ns/iter",
            "extra": "iterations: 92288\ncpu: 7590.331234830085 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 3012.0213332076696,
            "unit": "ns/iter",
            "extra": "iterations: 233392\ncpu: 3011.830238397196 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 3058.3841097256213,
            "unit": "ns/iter",
            "extra": "iterations: 228643\ncpu: 3058.1135700633745 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3179.204829467103,
            "unit": "ns/iter",
            "extra": "iterations: 221101\ncpu: 3179.0554859543854 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 4363.74315982441,
            "unit": "ns/iter",
            "extra": "iterations: 160376\ncpu: 4363.2581495984405 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.3529400912516367,
            "unit": "ns/iter",
            "extra": "iterations: 518301481\ncpu: 1.3528987736000688 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 97.38731753093231,
            "unit": "us/iter",
            "extra": "iterations: 7193\ncpu: 97.38194856110107 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.543026282430507,
            "unit": "us/iter",
            "extra": "iterations: 35499\ncpu: 19.542752359221407 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 50.83898892604339,
            "unit": "us/iter",
            "extra": "iterations: 46054\ncpu: 15.665071329309116 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 53.854322378122156,
            "unit": "us/iter",
            "extra": "iterations: 44758\ncpu: 16.781888288127227 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 53.95803819832382,
            "unit": "us/iter",
            "extra": "iterations: 42960\ncpu: 16.38623158752329 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 63.77577652896665,
            "unit": "us/iter",
            "extra": "iterations: 40305\ncpu: 17.88962324773602 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 53.094411728065595,
            "unit": "us/iter",
            "extra": "iterations: 44952\ncpu: 16.11130094322839 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 535.4534460719041,
            "unit": "us/iter",
            "extra": "iterations: 1502\ncpu: 468.68752330226437 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 51.993977984332794,
            "unit": "us/iter",
            "extra": "iterations: 41743\ncpu: 15.942021800062278 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 102.79545157974682,
            "unit": "us/iter",
            "extra": "iterations: 22377\ncpu: 30.58627014345081 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 215.69541893382427,
            "unit": "us/iter",
            "extra": "iterations: 10880\ncpu: 64.0416061580884 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 410.3661889409553,
            "unit": "us/iter",
            "extra": "iterations: 5335\ncpu: 127.98661349578255 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.3297519637220738,
            "unit": "us/iter",
            "extra": "iterations: 488231\ncpu: 1.3296871685738918 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.310434641874377,
            "unit": "us/iter",
            "extra": "iterations: 558561\ncpu: 1.3057769912328296 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.2941656769847119,
            "unit": "us/iter",
            "extra": "iterations: 544451\ncpu: 1.2941273649970346 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 1.599040806228549,
            "unit": "us/iter",
            "extra": "iterations: 571947\ncpu: 1.5988927383131635 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.2696706857300921,
            "unit": "us/iter",
            "extra": "iterations: 553031\ncpu: 1.2696157973061226 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 3.38570178171957,
            "unit": "us/iter",
            "extra": "iterations: 240049\ncpu: 3.385195564238972 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 11.047128943189445,
            "unit": "us/iter",
            "extra": "iterations: 69899\ncpu: 10.10971051088001 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 31.506722105616674,
            "unit": "us/iter",
            "extra": "iterations: 23822\ncpu: 29.53249819494602 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 67.50346056858368,
            "unit": "us/iter",
            "extra": "iterations: 12452\ncpu: 62.264073000321346 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.2510997341184964,
            "unit": "us/iter",
            "extra": "iterations: 560400\ncpu: 1.251023576017134 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 50.79496713428603,
            "unit": "us/iter",
            "extra": "iterations: 45336\ncpu: 15.301734427386663 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 52.442780756918594,
            "unit": "us/iter",
            "extra": "iterations: 44047\ncpu: 15.660287715395013 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 62.58486035037006,
            "unit": "us/iter",
            "extra": "iterations: 40129\ncpu: 17.397279997009612 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 94.01285051017328,
            "unit": "us/iter",
            "extra": "iterations: 33126\ncpu: 21.81367309666132 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 19.721752883464156,
            "unit": "us/iter",
            "extra": "iterations: 68494\ncpu: 9.874023374310099 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 21.361375910671896,
            "unit": "us/iter",
            "extra": "iterations: 54630\ncpu: 10.615043327841931 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 25.524256130346245,
            "unit": "us/iter",
            "extra": "iterations: 49100\ncpu: 13.626500162932896 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 48.52932094624372,
            "unit": "us/iter",
            "extra": "iterations: 22658\ncpu: 30.26852078735994 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 29.055633536055588,
            "unit": "us/iter",
            "extra": "iterations: 51795\ncpu: 13.412255545902154 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 31.330894968527335,
            "unit": "us/iter",
            "extra": "iterations: 48614\ncpu: 14.928020138231698 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 30.228979435925854,
            "unit": "us/iter",
            "extra": "iterations: 36520\ncpu: 15.207156736035147 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 57.393765513785134,
            "unit": "us/iter",
            "extra": "iterations: 19950\ncpu: 34.14971979949897 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 56.73321797790334,
            "unit": "us/iter",
            "extra": "iterations: 18011\ncpu: 34.78890244850393 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 32.434867476016926,
            "unit": "us/iter",
            "extra": "iterations: 23558\ncpu: 22.26712068087288 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 52.59824477519071,
            "unit": "us/iter",
            "extra": "iterations: 19283\ncpu: 31.40879930508721 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 61.05366101695027,
            "unit": "us/iter",
            "extra": "iterations: 14632\ncpu: 45.12073011208306 us\nthreads: 1"
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
          "id": "a79513e84aa1e017b62730b3ff95289d8a62e030",
          "message": "Add 12 platform-specific IPC examples (#64)\n\n* Add file-watcher example with inotify-backed server and tests\n\nLinux directory watcher using inotify that demonstrates IPC notifications.\nServer watches directories and broadcasts FileChanged events to clients.\nSeven integration tests cover watch/unwatch lifecycle, event notifications\nfor file creation/modification/deletion, and error handling.\n\nCo-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>\n\n* Add sysmon example with /proc-based CPU, memory, and load monitoring\n\nServer reads /proc/stat, /proc/meminfo, and /proc/loadavg to implement\nGetCpuUsage, GetMemoryInfo, and GetLoadAverage RPCs. A monitoring thread\nchecks thresholds every second and fires HighCpuAlert/LowMemoryAlert\nnotifications.\n\nClient queries all three methods in a formatted table and listens for\nthreshold notifications. Test suite covers all RPCs, both notification\ntypes, invalid method handling, and RunLoop dispatch mode (8 tests).\n\nCo-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>\n\n* Add disk-usage example with simulated volumes\n\nImplement the disk-usage example under examples/macos/disk-usage/:\n\n- disk_device.cpp: Simulated server with 3 volumes (/, /home, /tmp).\n  Usage on / grows 1 GB every 3s; fires DiskSpaceLow when > 90%.\n- disk_client.cpp: Interactive CLI with list, check <mount>, quit\n  commands and bar-chart display. Prints notification alerts.\n- disk_test.cpp: 8 Google Test cases covering GetVolumeCount,\n  GetVolumeAt (valid/invalid), GetDiskUsage (known/unknown mount),\n  DiskSpaceLow notification, UsedPercent consistency, RunLoop dispatch.\n- CMakeLists.txt: Builds device, client, and test executables.\n- README.md: IDL overview, build/run instructions, sample output.\n\nAll volumes are simulated — no platform-specific APIs — so the example\ncompiles and runs on Linux, macOS, and Windows.\n\nCo-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>\n\n* Add audio-dsp example: simulated DSP effects processor\n\nImplement server (dsp_device), client CLI (dsp_dashboard), and 12 Google\nTest cases covering slot management, parameter updates, effect counting,\nsignal levels, invalid-slot errors, and ClipDetected/LevelUpdate\nnotifications.\n\nCo-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>\n\n* Add can-bus-ecu example: simulated ECU with OBD-II PIDs\n\nImplement ecu_device (server with state machine simulation),\necu_dashboard (interactive CLI client), and ecu_test (11 Google\nTest cases covering all RPC methods, notifications, and RunLoop\ndispatch). Cross-platform — compiles on Linux and Windows.\n\nCo-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>\n\n* Add BLE Heart Rate Monitor peripheral example\n\nImplement simulated BLE HRM with GATT service model:\n- ble_device: server with Heart Rate (0x180D) and Battery (0x180F)\n  services, simulation thread for HR/battery updates\n- ble_dashboard: interactive CLI client with service discovery,\n  characteristic read, and notification subscription\n- ble_test: 9 Google Test cases covering service count, get/read/\n  write characteristics, notification enable/disable, error cases\n\nAll code is cross-platform (no macOS-specific APIs).\n\nCo-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>\n\n* Add motor-controller example with simulated stepper motor\n\nImplements MotorController IPC service with trapezoidal motion profiles,\nlimit switch detection, stall monitoring, and homing sequence. Includes:\n\n- motor_device: simulated stepper motor server (10ms tick loop)\n- motor_dashboard: interactive CLI client with status display\n- motor_test: 10 Google Test cases covering all methods and notifications\n\nAll code is cross-platform (no Windows-specific APIs).\n\nCo-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>\n\n* Fix CMake wiring and build warnings for platform examples\n\n- Consolidate platform subdirectory includes in examples/CMakeLists.txt\n- Guard Linux-only examples (sysmon, file-watcher) inside linux/CMakeLists.txt\n- Remove duplicate add_subdirectory entries from parallel agent commits\n- Fix -Werror=unused-result warnings in filewatcher_test.cpp\n\n* Address code review findings across platform examples\n\n- Fix dead code and wrong formula in battery_device timeToEmpty calc\n- Move notifyXxx calls outside mutex locks in motor, gpio, procmon, svcstat\n- Add can-bus-ecu to windows/CMakeLists.txt (was missing)\n- Replace fragile reference capture in ble_device with member m_simRunning\n- Add explicit null termination after strncpy in disk_device\n- Move ecu_device printf inside lock scope to avoid data race\n\n* Fix motor simulation crossover on coarse-timer platforms\n\nOn Windows (~15ms timer resolution) and macOS, a single simulation\ntick can overshoot the MoveTo target position. Add post-update\ncrossover detection in both the production device and test device\nto snap to target when position crosses or reaches the destination.\n\nFixes CI failures: MoveTo_ReachesTarget, MoveWithoutHome_Allowed\n\n---------\n\nCo-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>",
          "timestamp": "2026-03-28T19:35:31+11:00",
          "tree_id": "fb49bce97336071a6a4698567b8e6f643d46484e",
          "url": "https://github.com/Mrunmoy/aether/commit/a79513e84aa1e017b62730b3ff95289d8a62e030"
        },
        "date": 1774687069600,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 63196.39841854545,
            "unit": "ns/iter",
            "extra": "iterations: 11129\ncpu: 63098.104322041516 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 63214.23663642071,
            "unit": "ns/iter",
            "extra": "iterations: 11131\ncpu: 63117.352169616395 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 63149.29461067015,
            "unit": "ns/iter",
            "extra": "iterations: 11096\ncpu: 63044.1698810382 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 63570.65421152618,
            "unit": "ns/iter",
            "extra": "iterations: 11053\ncpu: 63451.60363702159 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 63940.732614456414,
            "unit": "ns/iter",
            "extra": "iterations: 10943\ncpu: 63903.403454262996 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2938.4472251396733,
            "unit": "ns/iter",
            "extra": "iterations: 238030\ncpu: 2937.9654077217156 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 2972.707111340107,
            "unit": "ns/iter",
            "extra": "iterations: 223235\ncpu: 2972.0320559052097 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3056.626867495646,
            "unit": "ns/iter",
            "extra": "iterations: 229117\ncpu: 3055.96533212289 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4359.015018112453,
            "unit": "ns/iter",
            "extra": "iterations: 160939\ncpu: 4358.027146931442 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7365.4043046150055,
            "unit": "ns/iter",
            "extra": "iterations: 95386\ncpu: 7363.882729121667 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2940.630635445776,
            "unit": "ns/iter",
            "extra": "iterations: 235740\ncpu: 2940.188924238565 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 2970.6595806998293,
            "unit": "ns/iter",
            "extra": "iterations: 235583\ncpu: 2970.418604058868 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3050.261502940539,
            "unit": "ns/iter",
            "extra": "iterations: 229550\ncpu: 3050.077660640381 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 4068.688040129589,
            "unit": "ns/iter",
            "extra": "iterations: 163271\ncpu: 4068.264633645901 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.3531616941368554,
            "unit": "ns/iter",
            "extra": "iterations: 518374368\ncpu: 1.3530251364589068 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 95.66339980863964,
            "unit": "us/iter",
            "extra": "iterations: 7316\ncpu: 95.65510196828859 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.013901107272172,
            "unit": "us/iter",
            "extra": "iterations: 36757\ncpu: 19.012437494898933 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 45.95784795936239,
            "unit": "us/iter",
            "extra": "iterations: 51381\ncpu: 13.783825402386096 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 46.38884906622347,
            "unit": "us/iter",
            "extra": "iterations: 52957\ncpu: 13.48309855165512 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 47.408216946690494,
            "unit": "us/iter",
            "extra": "iterations: 52317\ncpu: 14.405642945887536 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 56.06436253591328,
            "unit": "us/iter",
            "extra": "iterations: 47336\ncpu: 14.686403266013192 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 46.92327054166266,
            "unit": "us/iter",
            "extra": "iterations: 51785\ncpu: 14.136785845321972 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 493.8348203477123,
            "unit": "us/iter",
            "extra": "iterations: 1553\ncpu: 450.5747379265946 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 47.19382816271611,
            "unit": "us/iter",
            "extra": "iterations: 50542\ncpu: 13.445760159867053 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 94.11660143815517,
            "unit": "us/iter",
            "extra": "iterations: 22807\ncpu: 28.10958596045068 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 193.14959432357327,
            "unit": "us/iter",
            "extra": "iterations: 11169\ncpu: 57.20940119974902 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 385.9265428709054,
            "unit": "us/iter",
            "extra": "iterations: 6228\ncpu: 114.35380651894678 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.2698968016111325,
            "unit": "us/iter",
            "extra": "iterations: 545212\ncpu: 1.2697321262187953 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.2471674632059688,
            "unit": "us/iter",
            "extra": "iterations: 526444\ncpu: 1.2470777328642721 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.302811554148936,
            "unit": "us/iter",
            "extra": "iterations: 538404\ncpu: 1.3026642316921848 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 0.05201842778284045,
            "unit": "us/iter",
            "extra": "iterations: 38256420\ncpu: 0.0520128976260716 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.2559913303697956,
            "unit": "us/iter",
            "extra": "iterations: 552042\ncpu: 1.2558797029936102 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 3.5887256029075245,
            "unit": "us/iter",
            "extra": "iterations: 203887\ncpu: 3.5880552462883806 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 10.50437173916068,
            "unit": "us/iter",
            "extra": "iterations: 72029\ncpu: 9.603095711449575 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 30.334097840301222,
            "unit": "us/iter",
            "extra": "iterations: 25235\ncpu: 28.363429799881082 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 65.44119182737214,
            "unit": "us/iter",
            "extra": "iterations: 11771\ncpu: 59.91296236513456 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.247397274412815,
            "unit": "us/iter",
            "extra": "iterations: 532876\ncpu: 1.2473626772457351 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 46.80431936461855,
            "unit": "us/iter",
            "extra": "iterations: 53763\ncpu: 13.56577984859481 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 46.95859607187295,
            "unit": "us/iter",
            "extra": "iterations: 49031\ncpu: 13.779900940221454 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 57.59845232883233,
            "unit": "us/iter",
            "extra": "iterations: 46139\ncpu: 15.33790871063529 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 83.59285636789546,
            "unit": "us/iter",
            "extra": "iterations: 40555\ncpu: 17.558253680187498 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 18.418607954545404,
            "unit": "us/iter",
            "extra": "iterations: 79552\ncpu: 9.120693584070784 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 17.950835837123073,
            "unit": "us/iter",
            "extra": "iterations: 75419\ncpu: 8.871130855619935 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 22.772000268614867,
            "unit": "us/iter",
            "extra": "iterations: 55842\ncpu: 13.399846567100031 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 40.09376636893832,
            "unit": "us/iter",
            "extra": "iterations: 27094\ncpu: 26.161960323318926 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 26.72313988188221,
            "unit": "us/iter",
            "extra": "iterations: 60787\ncpu: 12.021317008571 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 28.011034768423634,
            "unit": "us/iter",
            "extra": "iterations: 62528\ncpu: 12.975052280578334 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 26.918485986381917,
            "unit": "us/iter",
            "extra": "iterations: 44207\ncpu: 14.141184224217879 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 58.33827543877888,
            "unit": "us/iter",
            "extra": "iterations: 19144\ncpu: 36.57556513790234 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 47.057568641474596,
            "unit": "us/iter",
            "extra": "iterations: 25167\ncpu: 28.141137282949835 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 32.17252114865868,
            "unit": "us/iter",
            "extra": "iterations: 31515\ncpu: 22.292076852292407 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 52.70284549320392,
            "unit": "us/iter",
            "extra": "iterations: 23986\ncpu: 33.07601108980238 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 63.04778877197556,
            "unit": "us/iter",
            "extra": "iterations: 15301\ncpu: 46.442068230834764 us\nthreads: 1"
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
          "id": "9c44c4ec1aa08233a3d8339a8d0eddcc6bc341cf",
          "message": "Update examples README with platform-specific examples (#65)",
          "timestamp": "2026-03-28T19:43:34+11:00",
          "tree_id": "7b6da6431085b87e41183eb6d89015613063eb8a",
          "url": "https://github.com/Mrunmoy/aether/commit/9c44c4ec1aa08233a3d8339a8d0eddcc6bc341cf"
        },
        "date": 1774687547066,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 61726.177254901326,
            "unit": "ns/iter",
            "extra": "iterations: 11475\ncpu: 61692.20531590414 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 61014.59383483613,
            "unit": "ns/iter",
            "extra": "iterations: 11419\ncpu: 60983.78404413698 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 61250.88372092981,
            "unit": "ns/iter",
            "extra": "iterations: 11395\ncpu: 61221.91180342255 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 61721.45211875547,
            "unit": "ns/iter",
            "extra": "iterations: 11351\ncpu: 61694.16624085985 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 63938.48722176375,
            "unit": "ns/iter",
            "extra": "iterations: 10917\ncpu: 63861.18512411836 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 2964.593770227177,
            "unit": "ns/iter",
            "extra": "iterations: 237922\ncpu: 2964.3488706382773 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 2986.506598252939,
            "unit": "ns/iter",
            "extra": "iterations: 235820\ncpu: 2985.9230133152396 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3032.221146446968,
            "unit": "ns/iter",
            "extra": "iterations: 230015\ncpu: 3031.4868812903533 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 3898.564651359446,
            "unit": "ns/iter",
            "extra": "iterations: 179741\ncpu: 3897.7786982380244 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 6936.339784477551,
            "unit": "ns/iter",
            "extra": "iterations: 100964\ncpu: 6935.729596687939 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 2933.8756126706207,
            "unit": "ns/iter",
            "extra": "iterations: 238505\ncpu: 2933.8135678497333 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 2968.964344071473,
            "unit": "ns/iter",
            "extra": "iterations: 236230\ncpu: 2968.736392498829 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3037.7518617504215,
            "unit": "ns/iter",
            "extra": "iterations: 231234\ncpu: 3037.4700519819753 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 3896.554808798757,
            "unit": "ns/iter",
            "extra": "iterations: 179889\ncpu: 3896.2895007476814 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.3521150237714084,
            "unit": "ns/iter",
            "extra": "iterations: 517500472\ncpu: 1.3520449484730133 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 95.44827104078443,
            "unit": "us/iter",
            "extra": "iterations: 7331\ncpu: 95.44321770563364 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 19.000175101407432,
            "unit": "us/iter",
            "extra": "iterations: 36733\ncpu: 19.000141616530108 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 47.17306629621706,
            "unit": "us/iter",
            "extra": "iterations: 46790\ncpu: 14.581360525753377 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 47.06477249516111,
            "unit": "us/iter",
            "extra": "iterations: 48566\ncpu: 14.156112239014936 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 47.81952302586611,
            "unit": "us/iter",
            "extra": "iterations: 51225\ncpu: 14.07680054660809 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 57.36212952925722,
            "unit": "us/iter",
            "extra": "iterations: 43357\ncpu: 15.737313328874226 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 47.69807498904595,
            "unit": "us/iter",
            "extra": "iterations: 47927\ncpu: 14.51051532539068 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 485.73606599622804,
            "unit": "us/iter",
            "extra": "iterations: 1591\ncpu: 442.946185417977 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 48.777050975320634,
            "unit": "us/iter",
            "extra": "iterations: 46395\ncpu: 14.389791421489367 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 92.8806437560097,
            "unit": "us/iter",
            "extra": "iterations: 23919\ncpu: 27.90499314352616 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 196.39773359659745,
            "unit": "us/iter",
            "extra": "iterations: 13168\ncpu: 58.97956728432581 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 381.6301060266174,
            "unit": "us/iter",
            "extra": "iterations: 6687\ncpu: 114.33567339614204 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 1.3224385723698107,
            "unit": "us/iter",
            "extra": "iterations: 506938\ncpu: 1.3223293676938774 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 1.256136742951515,
            "unit": "us/iter",
            "extra": "iterations: 554588\ncpu: 1.2561209798985906 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.3107246226069793,
            "unit": "us/iter",
            "extra": "iterations: 535516\ncpu: 1.3103383577708252 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 0.009766320552946189,
            "unit": "us/iter",
            "extra": "iterations: 61268880\ncpu: 0.009763138415456625 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 1.272146248328363,
            "unit": "us/iter",
            "extra": "iterations: 558591\ncpu: 1.2720067115295421 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 3.260175091922915,
            "unit": "us/iter",
            "extra": "iterations: 164268\ncpu: 3.2600478486376003 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 10.373894847967126,
            "unit": "us/iter",
            "extra": "iterations: 71991\ncpu: 9.503740009167794 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 30.00728873902136,
            "unit": "us/iter",
            "extra": "iterations: 25504\ncpu: 28.214501725219563 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 65.94372869757154,
            "unit": "us/iter",
            "extra": "iterations: 13590\ncpu: 61.36117844002945 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 1.2485064595564705,
            "unit": "us/iter",
            "extra": "iterations: 553908\ncpu: 1.2482128530369598 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 46.78164415773591,
            "unit": "us/iter",
            "extra": "iterations: 48055\ncpu: 13.774107481011411 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 48.74197092988769,
            "unit": "us/iter",
            "extra": "iterations: 50533\ncpu: 14.724457146814975 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 57.1133067436812,
            "unit": "us/iter",
            "extra": "iterations: 47437\ncpu: 15.40775658241458 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 81.8466070270518,
            "unit": "us/iter",
            "extra": "iterations: 43176\ncpu: 16.383530039837073 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 17.31143653613451,
            "unit": "us/iter",
            "extra": "iterations: 77682\ncpu: 8.517646340207516 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 19.087244740997278,
            "unit": "us/iter",
            "extra": "iterations: 78532\ncpu: 9.486356402485596 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 21.534837667865666,
            "unit": "us/iter",
            "extra": "iterations: 57561\ncpu: 12.024466062090548 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 39.28420957983209,
            "unit": "us/iter",
            "extra": "iterations: 29750\ncpu: 25.219577815126033 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 27.94731195617136,
            "unit": "us/iter",
            "extra": "iterations: 58957\ncpu: 12.761701765693699 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 26.628745401354124,
            "unit": "us/iter",
            "extra": "iterations: 52298\ncpu: 11.916901219931942 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 30.148469455160306,
            "unit": "us/iter",
            "extra": "iterations: 39663\ncpu: 15.591788770390629 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 58.4883854714597,
            "unit": "us/iter",
            "extra": "iterations: 23003\ncpu: 33.700534452028215 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 48.24170071656657,
            "unit": "us/iter",
            "extra": "iterations: 25678\ncpu: 29.01852877949998 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 31.897955340367467,
            "unit": "us/iter",
            "extra": "iterations: 34595\ncpu: 22.018674114756617 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 50.597153812992865,
            "unit": "us/iter",
            "extra": "iterations: 23197\ncpu: 31.29984541104448 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 64.0754271629116,
            "unit": "us/iter",
            "extra": "iterations: 15315\ncpu: 47.08749924910222 us\nthreads: 1"
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
          "id": "dd07c65104e11e4949a9cab4c76598c25941ff2e",
          "message": "Merge pull request #68 from Mrunmoy/improve/canonical-example-path\n\nMake echo the canonical source-build example",
          "timestamp": "2026-03-30T15:41:51+11:00",
          "tree_id": "5c4250ea78483d0ce03a27f4af7f126867f0b923",
          "url": "https://github.com/Mrunmoy/aether/commit/dd07c65104e11e4949a9cab4c76598c25941ff2e"
        },
        "date": 1774845859052,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_WriteFrame/0",
            "value": 41147.59888810037,
            "unit": "ns/iter",
            "extra": "iterations: 16908\ncpu: 41108.65613910575 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/64",
            "value": 41104.22338733442,
            "unit": "ns/iter",
            "extra": "iterations: 16975\ncpu: 41064.49201767304 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/1024",
            "value": 41349.92609051622,
            "unit": "ns/iter",
            "extra": "iterations: 17102\ncpu: 41302.36738393168 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/16384",
            "value": 41277.80329994157,
            "unit": "ns/iter",
            "extra": "iterations: 16970\ncpu: 41217.85509723038 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteFrame/65536",
            "value": 42780.02620220032,
            "unit": "ns/iter",
            "extra": "iterations: 16449\ncpu: 42735.79883275577 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/0",
            "value": 3936.5440659106384,
            "unit": "ns/iter",
            "extra": "iterations: 177938\ncpu: 3935.864087491151 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/64",
            "value": 3953.055945952051,
            "unit": "ns/iter",
            "extra": "iterations: 177028\ncpu: 3952.6088076462515 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/1024",
            "value": 3980.611475643457,
            "unit": "ns/iter",
            "extra": "iterations: 175415\ncpu: 3980.2065957871355 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/16384",
            "value": 4541.899127130313,
            "unit": "ns/iter",
            "extra": "iterations: 154204\ncpu: 4541.292015771326 ns\nthreads: 1"
          },
          {
            "name": "BM_ReadFrameAlloc/65536",
            "value": 7916.390694414624,
            "unit": "ns/iter",
            "extra": "iterations: 88463\ncpu: 7915.391813526571 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/0",
            "value": 3934.620945767538,
            "unit": "ns/iter",
            "extra": "iterations: 177993\ncpu: 3934.2319473237685 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/64",
            "value": 3953.2838380873736,
            "unit": "ns/iter",
            "extra": "iterations: 177182\ncpu: 3952.8323193100878 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/1024",
            "value": 3989.0876631497335,
            "unit": "ns/iter",
            "extra": "iterations: 175912\ncpu: 3988.5066396834773 ns\nthreads: 1"
          },
          {
            "name": "BM_WriteReadRoundTrip/16384",
            "value": 4541.306276677141,
            "unit": "ns/iter",
            "extra": "iterations: 154158\ncpu: 4540.66161989647 ns\nthreads: 1"
          },
          {
            "name": "BM_PeekFrameHeader",
            "value": 1.3518734668925076,
            "unit": "ns/iter",
            "extra": "iterations: 506201873\ncpu: 1.3517077701527984 ns\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/64",
            "value": 89.34399272123517,
            "unit": "us/iter",
            "extra": "iterations: 7831\ncpu: 89.33050402247471 us\nthreads: 1"
          },
          {
            "name": "BM_MultiFramePipeline/1024",
            "value": 20.463823154323084,
            "unit": "us/iter",
            "extra": "iterations: 34188\ncpu: 20.46160167310173 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/0",
            "value": 38.241082884695835,
            "unit": "us/iter",
            "extra": "iterations: 76528\ncpu: 9.51708006219947 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/64",
            "value": 38.541428222512316,
            "unit": "us/iter",
            "extra": "iterations: 71241\ncpu: 9.431312685111127 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/1024",
            "value": 39.48891705998269,
            "unit": "us/iter",
            "extra": "iterations: 65469\ncpu: 9.727912890070122 us\nthreads: 1"
          },
          {
            "name": "BM_CallLatency/16384",
            "value": 43.30199834903209,
            "unit": "us/iter",
            "extra": "iterations: 57542\ncpu: 10.526211445552839 us\nthreads: 1"
          },
          {
            "name": "BM_CallThroughput",
            "value": 38.14409812429069,
            "unit": "us/iter",
            "extra": "iterations: 59924\ncpu: 9.084458430678815 us\nthreads: 1"
          },
          {
            "name": "BM_ConnectDisconnect",
            "value": 352.3488329519441,
            "unit": "us/iter",
            "extra": "iterations: 2185\ncpu: 314.1575867276891 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/1",
            "value": 39.07042431490541,
            "unit": "us/iter",
            "extra": "iterations: 77763\ncpu: 10.00873682856883 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/2",
            "value": 80.5392551613718,
            "unit": "us/iter",
            "extra": "iterations: 35601\ncpu: 20.007600910086875 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/4",
            "value": 161.41517335501482,
            "unit": "us/iter",
            "extra": "iterations: 17219\ncpu: 39.92154463093084 us\nthreads: 1"
          },
          {
            "name": "BM_ParallelClients/8",
            "value": 320.94191366906415,
            "unit": "us/iter",
            "extra": "iterations: 8618\ncpu: 79.33063413785123 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/0",
            "value": 0.9111266657751238,
            "unit": "us/iter",
            "extra": "iterations: 762708\ncpu: 0.9110449320054372 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/64",
            "value": 0.9191142797426123,
            "unit": "us/iter",
            "extra": "iterations: 755952\ncpu: 0.9190618160941425 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/1024",
            "value": 1.0183994432636496,
            "unit": "us/iter",
            "extra": "iterations: 697278\ncpu: 1.0183026454297976 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyLatency/16384",
            "value": 0.02196313834799115,
            "unit": "us/iter",
            "extra": "iterations: 31396531\ncpu: 0.021959227183410823 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/1",
            "value": 0.9054829798562303,
            "unit": "us/iter",
            "extra": "iterations: 753137\ncpu: 0.9054032891758076 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/2",
            "value": 2.6256894509832325,
            "unit": "us/iter",
            "extra": "iterations: 287259\ncpu: 2.6253808096526132 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/4",
            "value": 8.361512110808892,
            "unit": "us/iter",
            "extra": "iterations: 84181\ncpu: 8.050176429360546 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/8",
            "value": 22.71213465571994,
            "unit": "us/iter",
            "extra": "iterations: 33374\ncpu: 21.811126026248036 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyBroadcast/16",
            "value": 47.37681701646339,
            "unit": "us/iter",
            "extra": "iterations: 15914\ncpu: 45.369890285283645 us\nthreads: 1"
          },
          {
            "name": "BM_NotifyThroughput",
            "value": 0.9196327218768233,
            "unit": "us/iter",
            "extra": "iterations: 752084\ncpu: 0.9195371088867674 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/64",
            "value": 38.447066313655014,
            "unit": "us/iter",
            "extra": "iterations: 68553\ncpu: 9.823441687453453 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/1024",
            "value": 38.322086344983745,
            "unit": "us/iter",
            "extra": "iterations: 75789\ncpu: 9.295603082241522 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/16384",
            "value": 42.492308481756766,
            "unit": "us/iter",
            "extra": "iterations: 67038\ncpu: 10.643146006742427 us\nthreads: 1"
          },
          {
            "name": "BM_Aether_RoundTrip/65536",
            "value": 51.31668584592331,
            "unit": "us/iter",
            "extra": "iterations: 65331\ncpu: 11.49556481609028 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/64",
            "value": 15.120929392989549,
            "unit": "us/iter",
            "extra": "iterations: 87725\ncpu: 7.250854078084976 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/1024",
            "value": 15.46249701187248,
            "unit": "us/iter",
            "extra": "iterations: 87513\ncpu: 7.795584096077141 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/16384",
            "value": 14.217643232830925,
            "unit": "us/iter",
            "extra": "iterations: 73199\ncpu: 8.280147556660609 us\nthreads: 1"
          },
          {
            "name": "BM_UDS_RoundTrip/65536",
            "value": 27.248021615949956,
            "unit": "us/iter",
            "extra": "iterations: 38120\ncpu: 18.089950760755567 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/64",
            "value": 21.837121191156463,
            "unit": "us/iter",
            "extra": "iterations: 99735\ncpu: 7.804459056499745 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/1024",
            "value": 22.37554490213711,
            "unit": "us/iter",
            "extra": "iterations: 92323\ncpu: 8.108763894154206 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/16384",
            "value": 25.45613953368166,
            "unit": "us/iter",
            "extra": "iterations: 58029\ncpu: 10.791405676472136 us\nthreads: 1"
          },
          {
            "name": "BM_Pipe_RoundTrip/65536",
            "value": 31.827783508158433,
            "unit": "us/iter",
            "extra": "iterations: 30888\ncpu: 23.63477256539742 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/64",
            "value": 33.477476422806426,
            "unit": "us/iter",
            "extra": "iterations: 38533\ncpu: 18.552212545091347 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/1024",
            "value": 26.354721789972544,
            "unit": "us/iter",
            "extra": "iterations: 43442\ncpu: 16.188188734404587 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/16384",
            "value": 34.62228834440423,
            "unit": "us/iter",
            "extra": "iterations: 36283\ncpu: 19.802640051814876 us\nthreads: 1"
          },
          {
            "name": "BM_TCP_RoundTrip/65536",
            "value": 44.770953286232626,
            "unit": "us/iter",
            "extra": "iterations: 21514\ncpu: 31.5712136748166 us\nthreads: 1"
          }
        ]
      }
    ]
  }
}