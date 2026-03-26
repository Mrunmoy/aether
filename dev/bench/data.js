window.BENCHMARK_DATA = {
  "lastUpdate": 1774527215093,
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
      }
    ]
  }
}