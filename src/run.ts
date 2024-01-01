

import * as crypto from './crypto-typed'
import * as navierStokes from './navier-stokes-typed';
import * as rayTrace from './raytrace-typed';
import * as richards from './richards-typed';
import * as splay from './splay-typed';

const benchmarks = {
	crypto,
	navierStokes,
	rayTrace,
	richards,
	splay
}



for (const b in benchmarks) {
	// const bench = new Benchmark(b.benchmark_fn, { setup: b.setup_fn, teardown: b.teardown_fn });
	
	// bench.run();
	// console.log("nsamples: " + bench.stats.sample.length);
	// console.log("mean: " + bench.stats.mean);
	// console.log("moe: " + bench.stats.moe);
	// console.log("rme: " + bench.stats.rme);
	// console.log("std dev: " + bench.stats.deviation);
	// console.log("sem: " + bench.stats.sem);
	const startTime = process.hrtime();
		for (let i = 0; i < 10000; i++) {
			benchmarks[b].benchmark_fn();
		}
		const duration = process.hrtime(startTime);
		console.log(`${b}: ${duration[0]}s ${Math.round(duration[1] / 1000000)} ms`);
};

