/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   GenesysShell.cpp
 * Author: rafael.luiz.cancian
 *
 * Created on 23 de Maio de 2019, 13:02
 */

#include "GenesysConsole.h"
#include "Simulator.h"
#include "Assign.h"
#include <regex>
#include <fstream>
#include <assert.h>

#include "ProbDistribDefaultImpl1.h"

GenesysConsole::GenesysConsole() {
	_commands->setSortFunc([](const ShellCommand* a, const ShellCommand * b) {
		return a->shortname < b->shortname;
	});
	_commands->insert(new ShellCommand("q", "quit", "", "Quit ReGenesys shell. Same as exit.", DefineExecuterMember<GenesysConsole>(this, &GenesysConsole::cmdQuit)));
	_commands->insert(new ShellCommand("x", "exit", "", "Exit ReGenesys shell. Same as quit.", DefineExecuterMember<GenesysConsole>(this, &GenesysConsole::cmdQuit)));
	_commands->insert(new ShellCommand("h", "help", "", "Show help for commands.", DefineExecuterMember<GenesysConsole>(this, &GenesysConsole::cmdHelp)));
	_commands->insert(new ShellCommand("ms", "modelsave", "<filename>", "Save model.", DefineExecuterMember<GenesysConsole>(this, &GenesysConsole::cmdModelSave)));
	_commands->insert(new ShellCommand("ml", "modelload", "<filename>", "Load Model.", DefineExecuterMember<GenesysConsole>(this, &GenesysConsole::cmdModelLoad)));
	_commands->insert(new ShellCommand("rf", "readfile", "<filename>", "Read and execute shell command from file.", DefineExecuterMember<GenesysConsole>(this, &GenesysConsole::cmdScript)));
	_commands->insert(new ShellCommand("v", "version", "", "Show the version.", DefineExecuterMember<GenesysConsole>(this, &GenesysConsole::cmdVersion)));
	_commands->insert(new ShellCommand("ss", "start", "", "Start simulation.", DefineExecuterMember<GenesysConsole>(this, &GenesysConsole::cmdStart)));
	_commands->insert(new ShellCommand("mc", "modelcheck", "", "Check model.", DefineExecuterMember<GenesysConsole>(this, &GenesysConsole::cmdModelCheck)));
	_commands->insert(new ShellCommand("mh", "modelshow", "", "Show model.", DefineExecuterMember<GenesysConsole>(this, &GenesysConsole::cmdModelShow)));
	_commands->insert(new ShellCommand("ps", "step", "", "step simulation.", DefineExecuterMember<GenesysConsole>(this, &GenesysConsole::cmdStep)));
	_commands->insert(new ShellCommand("ts", "stop", "", "Stop simulation.", DefineExecuterMember<GenesysConsole>(this, &GenesysConsole::cmdStop)));
	_commands->insert(new ShellCommand("sr", "showreport", "", "Show simulation report.", DefineExecuterMember<GenesysConsole>(this, &GenesysConsole::cmdShowReport)));
	_commands->insert(new ShellCommand("tl", "tracelevel", "<0|1|2|...|7>", "Set the trace level (the bigger the most verbose).", DefineExecuterMember<GenesysConsole>(this, &GenesysConsole::cmdTraceLevel)));
    // _simulator->setTracer( new ConsoleTraceManager( _simulator ) );
    // _simulator->getTracer()->addTraceHandler( printf );
    _simulator->getTracer()->addTraceHandler<GenesysConsole>(this, &GenesysConsole::_traceHandler);
	_simulator->getTracer()->addTraceReportHandler<GenesysConsole>(this, &GenesysConsole::_traceReportHandler);
	_simulator->getTracer()->addTraceSimulationHandler<GenesysConsole>(this, &GenesysConsole::_traceSimulationHandler);
	_simulator->getTracer()->addTraceErrorHandler<GenesysConsole>(this, &GenesysConsole::_traceErrorHandler);

}

void GenesysConsole::cmdTraceLevel() {
	Trace("Set trace level");
	try {
		int tlnum = std::stoi(_parameter);
		Util::TraceLevel tl = static_cast<Util::TraceLevel> (tlnum);
		_simulator->getTracer()->setTraceLevel(tl);
	} catch (...) {
		Trace("Error setting trace level");
	}
}

void GenesysConsole::cmdModelCheck() {
	try {
        if( _simulator->getModels()->current() != nullptr )
        {
            Trace("Check model");
            _simulator->getModels()->current()->check();
        }
        else
            Trace("Error no model loaded");
    } catch (...) {
		Trace("Error checking model");
	}
}

void GenesysConsole::cmdStart() {
	try {
        if( _simulator->getModels()->current() != nullptr )
        {
            Trace("Start simulation");
            _simulator->getModels()->current()->getSimulation()->start();
        }
        else
        {
            Trace("Error no model loaded");
        }
	} catch (...) {
		Trace("Error starting simulation");
	}
}

void GenesysConsole::cmdStep() {
	try {
        if( _simulator->getModels()->current() != nullptr )
        {
            Trace("Step simulation");
            _simulator->getModels()->current()->getSimulation()->step();
        }
        else
        {
            Trace("Error no model loaded");
        }
	} catch (...) {
		Trace("Error stepping simulation");
	}
}

void GenesysConsole::cmdStop() {
    try {
        if( _simulator->getModels()->current() != nullptr )
        {
            Trace("Stop simulation");
            _simulator->getModels()->current()->getSimulation()->stop();
        }
        else
        {
            Trace("Error no model loaded");
        }
	} catch (...) {
		Trace("Error stopping simulation");
	}
}

void GenesysConsole::cmdShowReport() {
	Trace("Show report");
	try {
		_simulator->getModels()->current()->getSimulation()->getReporter()->showSimulationStatistics();
	} catch (...) {
		Trace("Error showing reports");
	}
}

void GenesysConsole::cmdHelp() {
	ShellCommand* command;
	Trace("List of commands:");
	Trace(Util::SetW("Short", 6) + Util::SetW("Long", 12) + Util::SetW("Parameters", 15) + "Description");
	for (std::list<ShellCommand*>::iterator it = _commands->list()->begin(); it != _commands->list()->end(); it++) {
		//Trace("Unknown command. Type \"-h\" or \"help\" for help on possible commands.");
		command = (*it);
		Trace(Util::SetW(command->shortname, 6) + Util::SetW(command->longname, 12) + Util::SetW(command->parameters, 15) + command->descrition);
	}
}

void GenesysConsole::cmdQuit() {
	Trace("Quiting/Exiting. Goodbye");
	exit(0);
}

void GenesysConsole::cmdVersion() {
	Trace("ReGenesys Shell version 2019.0528");
}

void GenesysConsole::cmdModelLoad() {
	Trace("Model load");
	try {
		std::string filename = _parameter;
		// Model* model = new Model(this->_simulator);
        if( _simulator->getModels()->loadModel(filename) )
        {
            Trace("  Load of file " + filename + " ok."  );

        }
        else
            Trace("  Load of file " + filename + " failed."  );

	} catch (...) {
		Trace("Error loading");
	}
}

void GenesysConsole::cmdModelShow() {
	try {
        if( _simulator->getModels()->current() != nullptr )
        {
            Trace("Model Show");
            _simulator->getModels()->current()->show();
        }
        else
        {
            Trace("Error no model loaded");
        }
	} catch (...) {
		//        _commands
		Trace("   Error showing");
	}
}

void GenesysConsole::cmdModelSave() {
    try {
        if( _simulator->getModels()->current() != nullptr )
        {
            Trace("Saving model...");
            std::string filename = _parameter;
            if( filename != "" )
            {
                _simulator->getModels()->current()->save( filename );
                Trace("Model saved");
            }
            else
            {
                Trace( "Error empty filename" );
            }
        }
        else
            Trace("Error no model loaded");
    } catch (...) {
		Trace("Error saving model");
	}
}

void GenesysConsole::cmdScript() {
	std::string filename = this->_parameter;
	//List<std::string>* arguments = new List<std::string>();
	std::ifstream commandfile;
	std::string inputText;
	try {
		commandfile.open(filename);
		while (getline(commandfile, inputText)) {
			this->tryExecuteCommand(inputText, "", "", " ");
		}
		commandfile.close();
	} catch (...) {
		Trace("   Error scripting");
	}
}

void GenesysConsole::Trace(std::string message) {
	std::cout << message << std::endl;
}

void GenesysConsole::run(List<std::string>* commandlineArgs) {
	Trace("ReGenesys Shell is running. Type your command. For help, type the command \"h\" or \"help\".");
	std::string inputText; //, shortPrefix, longPrefix, separator;
	while (true) {
		if (!commandlineArgs->empty()) {
			inputText = commandlineArgs->front();
			commandlineArgs->pop_front();
			tryExecuteCommand(inputText, "-", "--", "=");
		} else {
			std::cout << _prompt << " ";
			//std::cin >> inputText;
            std::getline( std::cin, inputText );
            Trace( inputText );
			tryExecuteCommand(inputText, "", "", " ");

		}
	}
}

void GenesysConsole::tryExecuteCommand(std::string inputText, std::string shortPrefix, std::string longPrefix, std::string separator) {
	std::regex regex{R"([\s]+)"}; // split on space R"([\s]+)"
	std::sregex_token_iterator it{inputText.begin(), inputText.end(), regex, -1};
	std::vector<std::string> fields{it,{}};
	std::string typedCommandStr = fields[0];
	if (fields.size() > 1) { // its a comnd and a paramater in the form command=parameter
		assert(fields.size() == 2);
		_parameter = fields[1];
	} else {
		_parameter = "";
	}
	ShellCommand* command;
	bool found;
	std::transform(typedCommandStr.begin(), typedCommandStr.end(), typedCommandStr.begin(), ::tolower);
	if (typedCommandStr.substr(0, 1) != "#") {
		found = false;
		for (std::list<ShellCommand*>::iterator it = _commands->list()->begin(); it != _commands->list()->end(); it++) {
			//Trace("Unknown command. Type \"-h\" or \"help\" for help on possible commands.");
			command = (*it);
			if (typedCommandStr == shortPrefix + command->shortname || typedCommandStr == longPrefix + command->longname) {
				command->executer();
				found = true;
				break;
			}
		}
		if (!found) {
			Trace("Command \"" + typedCommandStr + "\" not found. Type \"h\" or \"help\" for help.");
		}
	}

}

void GenesysConsole::_traceHandler(TraceEvent e) {
    if( e.getTracelevel() < _simulator->getTracer()->getTraceLevel() )
    {
        if (e.getTracelevel() == Util::TraceLevel::errorFatal)
    		std::cout << "\033[1;31m" + e.getText() + "\033[0m" << std::endl;
    	else if (e.getTracelevel() == Util::TraceLevel::errorRecover)
    		std::cout << "\033[1;35m" + e.getText() + "\033[0m" << std::endl;
    	std::cout << e.getText() << std::endl;
    }
}

void GenesysConsole::_traceErrorHandler(TraceErrorEvent e) {
	std::cout << "\033[1;31m" + e.getText() + "\033[0m" << std::endl;
}

void GenesysConsole::_traceReportHandler(TraceEvent e) {
	std::cout << "\033[1;32m" + e.getText() + "\033[0m" << std::endl;
}

void GenesysConsole::_traceSimulationHandler(TraceSimulationEvent e) {
	std::cout << "\033[1;36m" + e.getText() + "\033[0m" << std::endl;
}

void GenesysConsole::_insertPlugins() {
    Util::TraceLevel tl = static_cast<Util::TraceLevel> (0);
    _simulator->getTracer()->setTraceLevel(tl);
	_simulator->getPlugins()->insert("create.so");
	_simulator->getPlugins()->insert("dispose.so");
	_simulator->getPlugins()->insert("decide.so");
	_simulator->getPlugins()->insert("batch.so");
	_simulator->getPlugins()->insert("separate.so");
	_simulator->getPlugins()->insert("assign.so");
	_simulator->getPlugins()->insert("record.so");
	_simulator->getPlugins()->insert("submodel.so");
	_simulator->getPlugins()->insert("entitytype.so");
	_simulator->getPlugins()->insert("entitygroup.so");
	_simulator->getPlugins()->insert("attribute.so");
	_simulator->getPlugins()->insert("counter.so");
	_simulator->getPlugins()->insert("queue.so");
	_simulator->getPlugins()->insert("set.so");
	_simulator->getPlugins()->insert("resource.so");
	_simulator->getPlugins()->insert("variable.so");
	_simulator->getPlugins()->insert("schedule.so");
	_simulator->getPlugins()->insert("entitygroup.so");
	// arena advanced process
	_simulator->getPlugins()->insert("delay.so");
	_simulator->getPlugins()->insert("dropoff.so");
	_simulator->getPlugins()->insert("hold.so");
	_simulator->getPlugins()->insert("match.so");
	_simulator->getPlugins()->insert("pickup.so");
	_simulator->getPlugins()->insert("read.so");
	_simulator->getPlugins()->insert("write.so");
	_simulator->getPlugins()->insert("release.so");
	_simulator->getPlugins()->insert("remove.so");
	_simulator->getPlugins()->insert("seize.so");
	_simulator->getPlugins()->insert("search.so");
	_simulator->getPlugins()->insert("signal.so");
	_simulator->getPlugins()->insert("store.so");
	_simulator->getPlugins()->insert("unstore.so");
	_simulator->getPlugins()->insert("expression.so");
	_simulator->getPlugins()->insert("failure.so");
	_simulator->getPlugins()->insert("file.so");
	_simulator->getPlugins()->insert("statisticscollector.so");
	_simulator->getPlugins()->insert("storage.so");
	// arena transfer station
	_simulator->getPlugins()->insert("enter.so");
	_simulator->getPlugins()->insert("leave.so");
	_simulator->getPlugins()->insert("pickstation.so");
	_simulator->getPlugins()->insert("route.so");
	_simulator->getPlugins()->insert("sequence.so");
	_simulator->getPlugins()->insert("station.so");
	// arena transfer conveyour
	_simulator->getPlugins()->insert("access.so");
	_simulator->getPlugins()->insert("exit.so");
	_simulator->getPlugins()->insert("start.so");
	_simulator->getPlugins()->insert("stop.so");
	_simulator->getPlugins()->insert("conveyour.so");
	_simulator->getPlugins()->insert("segment.so");
	// arena transfer transport
	_simulator->getPlugins()->insert("alocate.so");
	_simulator->getPlugins()->insert("free.so");
	_simulator->getPlugins()->insert("halt.so");
	_simulator->getPlugins()->insert("move.so");
	_simulator->getPlugins()->insert("request.so");
	_simulator->getPlugins()->insert("transporter.so");
	_simulator->getPlugins()->insert("distance.so");
	_simulator->getPlugins()->insert("network.so");
	_simulator->getPlugins()->insert("networklink.so");
	// others
	_simulator->getPlugins()->insert("dummy.so");
	_simulator->getPlugins()->insert("lsode.so");
	_simulator->getPlugins()->insert("biochemical.so");
	_simulator->getPlugins()->insert("markovchain.so");
	_simulator->getPlugins()->insert("cellularautomata.so");
	_simulator->getPlugins()->insert("cppforgenesys.so");
    tl = static_cast<Util::TraceLevel> (1);
    _simulator->getTracer()->setTraceLevel(tl);
}

int GenesysConsole::main(int argc, char** argv) {
	//double res;
	//for (double x = -3.0; x <= 3.0; x += 0.05) {
	//    res = ProbDistrib::tStudent(x, 0, 1, 100);
	//    std::cout << x << ": " << res << std::endl;
	//}
	//return 0;
    _insertPlugins();
	List<std::string>* commandlineArgs = new List<std::string>();
	for (unsigned short i = 1; i < argc; i++) {
		std::string arg = argv[i];
		commandlineArgs->insert(arg);
	}
	//commandlineArgs->insert("-rf=temp/script.txt");
	//commandlineArgs->insert("-ml=models/genesysmodel.txt");
	this->run(commandlineArgs);
	return 0;
}
