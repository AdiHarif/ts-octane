
import Formatter from 'njstrace/lib/formatter';


export class TraceFormatter extends Formatter{
    log = [];

    onEntry(trace) {
        this.log.push({
            name: trace.name,
            file: trace.file,
            line: trace.line,
            time: process.hrtime(),
            type: 'call',
        });
    }

    onExit(trace) {
        this.log.push({
            name: trace.name,
            file: trace.file,
            line: trace.line,
            time: process.hrtime(),
            type: 'ret',
        });
    }
}

