
import njstrace from 'njstrace';
import { TraceFormatter } from './trace-formatter';

njstrace.inject({
	formatter: new TraceFormatter(),
});

import * as crypto from './crypto-typed'
import * as navierStokes from './navier-stokes-typed';
import * as rayTrace from './raytrace-typed';
import * as richards from './richards-typed';
import * as splay from './splay-typed';

// const benchmarks = [
// 	{
// 		name: "Crypto",
// 		setup: Crypto.,
// 	}
// 	{
// 		name: "Splay",
// 		setup: Splay.SplaySetup,
// 		run: Splay.SplayRun,
// 		teardown: Splay.SplayTearDown
// 	},
// ];
// benchmarks.forEach(bench => {
// 	bench.setup();
// 	bench.run();
// 	bench.teardown();
// });

const benchmarks = [
	crypto,
	navierStokes,
	rayTrace,
	richards,
	splay
]

const startTime = process.hrtime();

benchmarks.forEach(b => {
	// const bench = new Benchmark(b.benchmark_fn, { setup: b.setup_fn, teardown: b.teardown_fn });

	// bench.run();
	// console.log("nsamples: " + bench.stats.sample.length);
	// console.log("mean: " + bench.stats.mean);
	// console.log("moe: " + bench.stats.moe);
	// console.log("rme: " + bench.stats.rme);
	// console.log("std dev: " + bench.stats.deviation);
	// console.log("sem: " + bench.stats.sem);
	for (let i = 0; i < 1; i++) {
		b.benchmark_fn();
	}
});

const duration = process.hrtime(startTime);
console.log("Duration: " + duration[0] + "s " + Math.round(duration[1] / 1000000) + "ms");
