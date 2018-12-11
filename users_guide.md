# chiSIM User Guide #

Revision 0.3 for chi_sim v0.4.2

December 2018

## Overview ##
chiSIM is a software framework for creating distributed models in which a population of persons moves between places and is then co-located with other persons in those places. What happens (e.g. disease transmission, information exchange, etc.) in those places is model specific. Conceptually, a chiSIM model consists of the following components:

* Persons who move between places
* Places that receive persons and in which interaction occurs.

Places when created are assigned to specific process ranks, and chiSIM provides the functionality to easily move the persons between process ranks, adding the moved person to the appropriate place.

### Requirements ###

* RepastHPC (https://repast.github.io/repast_hpc.html)
* Boost (https://www.boost.org)
* googletest (https://github.com/abseil/googletest) for the unit tests.

Note that RepastHPC also requires Boost, so if RepastHPC is installed then boost will be as well.

## The Code ##

The chiSIM code has 3 parts:

1. Core chiSIM code in `src/chi_sim`
2. Utility code for working with "MRSA" population format models in `src/mrsa`
3. Example model code that implements a simple model in which people move between places, but do nothing in those places in `example/model_src`. This example model uses a "MRSA" format popuation.

### Compiling ###

1. Create a directory in the top level (i.e. at the same level as `src`) such as a Release or Debug.
2. Copy Makefile.tmplt into the directory and rename to Makefile
3. cd into the created directory
3. Edit the Makefile for your machine
4. Make. Targets are:
    * model - compiles the sample model into `example/sample_model`
    * libchiSIM - compiles chiSIM as a library
    * tests - compiles the unit tests
    * install - compiles as a library and then copies that library and header files to a Makefile specified install directory.

The code will compile into an X/build directory where X is the directory created in step 1. Application binares will also be created in X. Note that Release and Debug directories are in .gitignore.

### Unit Tests ###
The `tests` target will compile the unit test executable: `unit_tests`. The unit tests need to be run using mpi with two processes:

 `mpirun -n 2 ./unit_tests`
 
 You can also run `./unit_tests -h` to see other command line options
 (e.g. how to select the test suite to run).

## Writing a chiSIM Model ##

A chiSIM model consists of at least 3 C++ classes that implement Persons, Places, and a Model-type class that provides overall simulation management, creating the persons and places, scheduling, and moving persons between processes. chiSIM provides an `AbstractModel` class that contains the functionality for moving persons between processes and an `AbstractPerson` class that contains some minimal functionality required for moving Persons between processes. Implementing a chiSIM model begins by extending these two classes.

### AbstractModel ###

`chi_sim::AbstractModel` is found in `src/chi_sim/AbstractModel.h`. Its primary responsiblity is to move persons between processses and add them to the appropriate place. AbstractModel uses the notion of *local* and *remote* persons and places. Local persons and places are those that currently reside on that AbstractModel's rank. Remote persons and places are those that exist on other ranks. When a person moves from one rank to another, AbstractModel moves the person instance from its local person map to the remote person map. The remote_person map is used as a kind of cache, such that when a person is moved back to a rank on which it used to exist, an
entirely new person instance doesn't have to be created, the existing *remote* person can be updated with the moved person data.

All this is accomplished in ```AbstractModel::movePersons```, which works roughly as follows:

* On each process, iterate through all of the persons local this process. For each person,
    - determine what place the person will go at time *t* by calling the person class' ```selectNextPlace``` method.
    - if the selected place is on the current process, then add the person to the selected place.
    - else if the selected place is not on the current process, then gather the data that encapsulates that person so it can be sent to the process where the next place exists. The data is gathered by calling ```fillSendData``` on the person instance. The person is removed from the set of local persons.
* Using MPI, send the person data to the appropriate processes.
* On each process, iterate through all the received person data. For each chunk of data that represents an individual person,
    - if that person exists in the remote persons cache, then update the cached person with the new data, move the person the to set of local persons, and add the person to the selected place. Subclasses of ```AbstractModel``` are responsible for updating the cached person with the new data by implementing the virtual method ```updatePerson```.
    - else if that person does not exist in the remote person cache, then create a new person instance, add that instance to the remote person cache and the set of local persons, and add the person to the selected place. Subclasses of ```AbstractModel``` are responsible for creating the new person with the new data by implementing the virtual method ```createPerson```.

Typicaly, ```AbstractModel::movePersons``` is called as part of some regularly scheduled model behavior implemented by a subclass. For example, a *step* type method might increment the calendar to advance time forward, call ```movePersons``` to move population to their correct places, and then iterate through those places, calling some place specific behavior such as disease transmission. For example,

```c++
void CRXModel::step() {
    cal.increment();
    movePersons(cal);

    for (auto& place : local_places) {
        place->runDiseaseTranmission();
        place->reset();
    }
}
```

Here `cal` is an instance of `chi_sim::Calendar`, see the Utilities section below for details.

#### Subclassing AbstractModel ####

```AbstractModel``` is an abstract class with 3 template arguments:

```c++
template<typename PersonType, typename PlaceType, typename PersonDataType>
class AbstractModel {
...
}
```

1. `PersonType` - the Person type used in the model
2. `PlaceType` - the Place type used in the model
3. `PersonDataType` - the data type to use when sending Persons between processes.

With respect to `PersonDataType`, when sending Persons between processes, a Person's data is encapsulated in a user provided type `PersonDataType`, typically a struct with
fields for those parts of the Person that needs to be transferred between processes.
Instances of this `PersonDataType` are transferred to the appropriate process where a Person is then reconstituted from that struct.

At the very least, the `PersonDataType` struct must have 3 fields:

1. unsigned int person_id
2. int place_id
3. int act_type

AbstractModel.h will populate these fields when moving agents between processes,
and expects the `PersonDataType` struct to contain them.

As an example, Model.cpp in `example/model_src`,

```c++
using AbsModelT = chi_sim::AbstractModel<SamplePerson, Place, SamplePersonData>;

class Model : public AbsModelT {
```

where SamplePersonData is:

```c++
struct SamplePersonData {

    unsigned int person_id;
    int place_id;
    int act_type;
    unsigned int weekend_acts;
    unsigned int weekday_acts;
    int places[24];
};
```

The first 3 fields in the struct are required by chiSIM while the rest are data
specific to the sample model person.

#### Creating the Population ####

`AbstractModel` contains the following protected fields that subclasses are expected to fill when creating the simulated population (persons and places).

```c++
protected:
	std::map<unsigned int, std::shared_ptr<PersonType>> local_persons;
	// contains all the places either as full LocalPlace or as placeholder of
  // PlaceType
	std::map<int, std::shared_ptr<PlaceType>> places;
	// All the local places
	std::vector<std::shared_ptr<PlaceType>> local_places;
```

* `local_persons` - key is unique person id, value is the Person (shared_ptr of PersonType) with that id. This map should contain all the persons local to the current process rank.
* `places` - key is unique place id, value is the Place (a shared_ptr to a PlaceType instance) with that id. This map should contain all the places in the model. See the Places section below for information on local and remote places.
* `local_places` - Same type of content as `places`, but containing only the PlaceType instances that are local to the current process.

These must be filled by subclasses during model initialization. For example, Model.cpp in `example/model_src` calls its `initializePopulation` method as part of its constructor. Typically, the person and place definitions will be read from some data source such as a csv file that also specifies the starting rank for persons and the permanent rank for places. Using this rank information, the persons and places can be created and added to the correct *local* or *remote* map.

The demo model `Model.cpp` uses some of the `mrsa` utility to code to create its persons and places. See `src/mrsa/PersonCreator.h` and `src/mrsa/PlaceCreator.h`. Here a `PersonCreator` instance is used to fill `local_persons` and a `PlaceCreator` is used to fill `places`. Additional code in `Model.cpp`'s `initializePopulation` method fills the `local_places` map.

```c++
for (auto iter : place_map) {
  places.insert(make_pair(iter.second->placeId().place_id, iter.second));
  if (iter.second->placeId().rank == rank_) {
    local_places.push_back(iter.second);
  }
}
```

#### AbstractModel Constructor ####
`AbstractModel`'s constructor:

```c++
AbstractModel(repast::Properties& props, MPI_Datatype mpi_person_type);
```

* props - the model properties. See the "main" section below for more on creating `Properties`.
* mpi_person_type - a user provided MPI_Datatype corresponding to the `PersonDataType`
template argument mentioned above.  

In its constructor `AbstractModel` will assign its process rank to a protected `rank_` field where it is available to subclasses. It will also will set the model stop time from a `stop.at` property specified in the `props` constructor argument. Lastly it will initialize a `Parameters` singleton (see the chiSIM utilities section below) from the `props` constructor argument.

The user provided MPI_Datatype is used in passing the `PersonDataType` structs
between processes. The MPI function `MPI_Type_create_struct` can be used to create an
MPI_Datatype for a `PersonDataType` struct.


Given this struct:

```c++
struct SamplePersonData {
    unsigned int person_id;
    int place_id;
    int act_type;
    unsigned int weekend_acts;
    unsigned int weekday_acts;
    int places[24];
};
```

The following will create an MPI_Datatype for `SamplePersonData`. This can be
used an example for other struct types. See the embedded comments below for
additional details.

```c++
void make_mpi_type(MPI_Datatype* dt) {
    // Sample PersonData has 6 fields and we need to provide
    // type, block length and displacement information for each one. The
    // order must remain consistent over the types, block_lengths, and displacement
    // arrays, i.e. the info for SamplePersonData.person_id should be the first element
    // in all the arrays, SamplePersonData.place_id should be the second and so on.
    int count = 6;

    // Specify an MPI type for each field in the struct. The
    // unsigned int of SamplePersonData.person_id corresponds to an MPI_UNSIGNED,
    // and so forth
    MPI_Datatype types[count] = { MPI_UNSIGNED, MPI_INT, MPI_INT, MPI_UNSIGNED, MPI_UNSIGNED,
            MPI_INT };

    // The block lengths specify the number of elements of each type for
    // each field in SamplePersonData. Note that SamplePersonData.places is an array of 24
    // elements, so the block length is 24.
    int block_lengths[count] = { 1, 1, 1, 1, 1, 24};

    // Lastly, we need to specify the displacements. offsetof can be
    // used to calculate the displacement.
    MPI_Aint displacements[count];
    displacements[0] = offsetof(SamplePersonData, person_id);
    displacements[1] = offsetof(SamplePersonData, place_id);
    displacements[2] = offsetof(SamplePersonData, act_type);
    displacements[3] = offsetof(SamplePersonData, weekend_acts);
    displacements[4] = offsetof(SamplePersonData, weekday_acts);
    displacements[5] = offsetof(SamplePersonData, places);

    // Call MPI_Type_create_struct to create the temporary type (tmp_type)
    MPI_Datatype tmp_type;
    MPI_Type_create_struct(count, block_lengths, displacements, types, &tmp_type);

    // Resize tmp_type to the final MPI_Datatype "dt" and commit it.
    // Not sure this is strictly necessary but see:
    // https://stackoverflow.com/questions/33618937/trouble-understanding-mpi-type-create-struct
    MPI_Aint lb, extent;
    MPI_Type_get_extent(tmp_type, &lb, &extent);
    MPI_Type_create_resized(tmp_type, lb, extent, dt);
    MPI_Type_commit(dt);
}
```

#### AbstractModel Pure Virtual Methods ####

`AbstractModel` has two pure virtual methods that need to be implemented by subclasses. As mentioned above these either create or update a person from the sent data.

```c++
/**
 * Create a person from the specified parameters. Called when a remote person
 * is passed to this rank and a person needs to be created.
 */
virtual std::shared_ptr<PersonType> createPerson(unsigned int p_id, PersonDataType& data) = 0;

/**
 * Update the state of the specified person with the data in data. Called
 * to update a person cached on this rank when it moves to this rank.
 */
 virtual void updatePerson(std::shared_ptr<PersonType>& person, PersonDataType& data) = 0;
```

`createPerson` has 2 arguments:

* `unsigned int p_id` - the unique id for the person to create
* `PersonDataType data` - the data struct from the person to be created on this process

 As an example, assume that our Person class `MyPerson` has 2 integer fields: `x` and `y`. These fields are moved between processes in a MyPersonData struct.

```c++
struct MyPersonData {
  unsigned int person_id;
  int place_id;
  int act_type;
  int x, y;
};
```

Recall that the first 3 fields are required by chiSIM.

The creation code then might look like:

```c++
std::shared_ptr<MyPerson> MyModel::createPerson(unsigned int p_id, MyPersonData& data) {
    return std::make_shared<MyPerson>(p_id, data.x, data.y);
}
```

`updatePerson` also has 3 arguments:

* `std::shared_ptr<PersonType>& person` - the person to update
* `PersonDataType data` - the updated data for the specified person

As an example, assume the same Person class as above, `MyPerson`,  with 2 integer fields: `x` and `y`, and the same `MyPersonData` struct. These structs are moved between processes and the update code might then look like:

```c++
void MyModel::updatePerson(std::shared_ptr<MyPerson>& person, MyPersonData& data) {
    int x = data.x;
    int y = data.y;
    person->updateXY(x, y);
}
```

### Persons ###

`AbstractModel` takes care of adding persons to their next places and moving moving persons between processes if that next place is not on the current process. To determine the next place, `AbstractModel` calls `selectNextPlace` on each Person instance. If the Person's next place is on a different process and thus the person needs to be moved, `AbstractModel` will determine this and then call `fillSendData` on the person instance to retrieve the data to send.

`AbstractPerson` in `src/chi_sim` contains a minimal person implementation that can be used as the base for a model specific person class.

#### Subclassing AbstractPerson ####

`AbstractPerson` is an abstract class with 2 template arguments.

```c++
template<typename PlaceType, typename PersonDataType>
class AbstractPerson {
  ...
}
```

* `PlaceType` - the place type used in the model
* `PersonDataType` - the data type to use when sending Persons between processes. The
same struct type used by AbstractModel.

`AbstractPerson`'s constructor has a single argument, specifying a unique id that identifies this person. `AbstractPerson`'s `id()` method returns this id.

```c++
AbstractPerson(unsigned int num_id);
```

* unsigned int num_id - a numeric id that uniquely identifies this person.

`AbstractPerson` has two virtual methods that need to be implemented by subclasses.

```c++
/**
 * Selects the next place that this AbstractPerson will go, given the
 * current Calendar time.
 *
 * @param cal
 * @param next_place updated with the next place that this AbstractPerson
 * will go.
 */
virtual void selectNextPlace(Calendar& cal, NextPlace<PlaceType>& next_place) = 0;

/**
 * Fills the specified PersonDataType with this AbstractPerson's data to be
 * sent to another process when this person is moved.
 *
 * @param send_data the PersonDataType to fill
 */
virtual void fillSendData(PersonDataType& send_data);
```

` void selectNextPlace(Calendar& cal, NextPlace<PlaceType>& next_place) ` takes two arguments:

1. `Calendar& cal` - a Calendar instance that contains the current time.
2. `NextPlace<PlaceType>& next_place` - an out parameter that is a NextPlace instance to which the next place this `AbstractPerson` is going to is assigned. That assigment should occur in this method.

`NextPlace<PlaceType>` is a struct:

```c++
template<typename PlaceType>
struct NextPlace {

	PlaceId id{-1, -1};
	PlaceType* place{nullptr};
	int act_type{0};

	int placeRank() {
		return id.rank;
	}

	int placeId() {
	    return id.place_id;
	}
};
```

When assigning a place to `NextPlace`, its 3 fields need to be set.

1. `PlaceId id` - a PlaceId is a simple struct consisting of two fields, a numeric id that identifies the place, and that place's process rank. For more details on `PlaceId` see the utilities section below.
2. `PlaceType* place` - a pointer to the next place.
3. `int act_type` - an optional `int` that specifies the type activity the person will be doing in the place.

A typical `selectNextPlace` implementation will use the `Calendar` to retrieve the current time and determine where the Persons should go next in some model specific way. The result of that determination will be a `PlaceId`, a `PlaceType*`, and optionally an activity type specifier. For example,

```c++
void SamplePerson::selectNextPlace(Calendar& calendar, NextPlace<Place>& next_act) {
    std::pair<PlaceId, Place> place_data =  calcWhereToGoNext(calendar);
    next_act.id = place_data.first;
    next_act.place = place_data.second.get();
    // unused so set to 0
    next_act.act_type = 0;
}
```

`void fillSendData(PersonDataType& send_data);` takes a single argument.

1. PersonDataType& send_data - `AbstractPerson` subclasses should set the fields on this PersonDataType with the data to send when moving the person to another process.

Note that this method is not pure virtual. `AbstractPerson`'s implementation adds the person's numeric id to `send_data`.

```c++
template<typename PlaceType, typename PersonDataType>
void AbstractPerson<PlaceType, PersonDataType>::fillSendData(PersonDataType& data) {
	data.person_id = id_;
}
```

Subclasses  should thus call `AbstractPerson<PlaceType, PersonDataType>::fillSendData` prior to
setting anything else in the `PersonDataType`

A typical `fillSendData` implementation will add whatever data that constitutes the person to the send data vector. As an example, assume that our Person class `SamplePerson` has 2 integer fields: `x` and `y`, and two places, *work* and *home* of `SamplePlace` type that it also uses in its `selectNextPlace`. Its `PersonDataType` struct would then be:

```c++
struct MyPersonData {
  unsigned int person_id;
  int place_id;
  int act_type;
  int x, y;
  int work, home;
};
```

`SamplePerson::fillSendData` might then look like:

```c++
void SamplePerson::fillSendData(MyPersonData& data) {
    AbstractPerson<SamplePlace, MyPersonData>::fillSendData(data);
    data.x = x;
    data.y = y;
    PlaceId id = work->id();
    data.work = id.place_id;
    id = home->id();
    data.home = id.place_id;
```

When this data is retrieved by the receiving rank, the model can recreate the sent person with the following code.

```c++
std::shared_ptr<MyPerson> SampleModel::createPerson(unsigned int p_id, MyPersonData& data) {
    int x = data.x;
    int y = data.y;
    // look up that work place in AbstractModel's places map
    std::shared_ptr<SamplePlace> work = places[data.work];
    std::shared_ptr<SamplePlace> home = places[data.home];
    return std::make_shared<MyPerson>(p_id, x, y, work, home);
}
```

### Places ###

When an agent is moved between processes via `AbstractModel.movePersons()`, it is then added to its *next place* as 
described above. Once all the agents have been moved, the model specific behavior that is dependent upon
agent co-location can occur. For example, agents in the same place can expose each other to a disease. 

#### Place Type Requirements ####

Both `AbstractModel` and `AbstractPerson` required a template parameter that specifies the place type. This place type
has two method implementation requirements:

```c++
const chi_sim::PlaceId placeId() const;
virtual void addPerson(const PersonPtr& person, int act_type);
```

The first of these returns the id for the place in the form of a `PlaceId` type which is essentially a
unique id for that place and the places process rank (for more details see PlaceId below). The second
of these is called to add the person to its next place passing the person and the act type
when selected by `AbstractPerson::selectNextPlace`. At the very least, a typical place implementation would
add the passed person to some container (e.g. a vector) in the place.


#### Local and Remote Places ####

Unlike person, chiSIM does not move places between processes. A place is created on a specific rank and then persons
are moved between processes and added to the the selected place on that rank. However, `AbstractModel` does expect to know at
least the `PlaceId` (i.e. id and rank) of all the places in the model. As mentioned above, `AbstractModel` has two place
related fields.

```c++
std::map<int, std::shared_ptr<PlaceType>> places;
std::vector<std::shared_ptr<PlaceType>> local_places;
```

The first of these is map where the key is the integer id of the place and the value
is the place with that id. This map should contain all the places. The second field is vector which
should contain only those places that are local to the  current process. It is only these places that 
are current to the local process to which persons will be added.

Typically, the remote places are implemented by some very minimal implementation of the expected
methods, essentially being just a container for the `PlaceId`. The local places, to which 
persons will actually be added, extend this minimal implementation adding the appropriate model and domain
specific behavior. The `AbstractModel`'s and `AbstractPerson`'s `PlaceType` template parameter 
is the minimal parent class implementation. The sample model has an example of this in its `Place` and `LocalPlace` 
classes. `Place` doesn't do much more than hold the `PlaceId` while `LocalPlace` actually acts as container
for the persons added to it. Additional model specific behavior can be implemented in subclasses of `LocalPlace`,
such as `School`, `Workplace` and so forth.

Because of this division into local and remote places and their separate classes, place creation will typically
check the current process against the assigned rank of the place, if they are equal then the place is local and
it is instantiated in the class for the local places. Otherwise, it is a remote place and created appropriately.
`create_places` in the `mrsa/PlaceCreator.h` source file is an example of how this is done. 

```c++
if (rank == my_rank || world_size == 1) {
			//std::cout << "rank: " << my_rank << " making local place " << id << std::endl;
			place_map.insert(make_pair(id, place_creator.createLocalPlace(id, rank, type, line)));
		} else {
			//std::cout << "rank: " << my_rank << " making non-local place " << id << " for rank " << rank << std::endl;
			place_map.insert(make_pair(id, place_creator.createRemotePlace(id, rank, line)));
		}
}
```

Note the check for a world size of 1. In that case, the model is being run on a single process, and thus
all the places are local as there will be no distribution of place and movement of persons among processes.

#### Place Behavior ####

A typical chiSIM model has at least two sorts of behavior. The first determines how
an agent selects its next place. The second determines what happens when agents are co-located in a particular place.
This second behavior is typically implemented in the local place class. For example, a place implementation could
have a `run` method that iterates over all the agents in that place to trigger disease exposure. As with
moving the agents between processes, this typically takes place in a model class' step or step-like method,
scheduled to execute every iteration of the model. For example,

```c++
void Model::step() {
	// increment the calendar
	cal.increment();
  // move persons 
	movePersons(cal);

  for (auto& place : local_places) {
    /// run the model specified co-located behavior
    place->run();
  }
}
```


### chiSIM Utilties ###

chiSIM includes some useful utility code.

#### CSVReader ####

The `chi_sim::CSVReader` class reads csv format files line by line, splitting each line on its comma separated elements and placing those elements in a vector. `CSVReader`'s constructor takes the file to read as an argument and its `next(std::vector<string>)` method reads the next line into the specified vector.

Sample code:

```c++
#include "chi_sim/CSVReader.h"
...
std::string my_file = "some_dir/some_file.csv";
chi_sim::CSVReader reader(my_file);
std::vector<std::string> line;

// read the header, assuming there is one
reader.next(line);

while (reader.next(line)) {
  int id = std::stoi(line[0]);
  std::string name = line(2);
  ...
}
```

#### Parameters ####

The `chi_sim::Parameters` class is a singleton that provides global access to model properties via a variety of `get*` methods. `chi_sim::AbstractModel` will initialize the `Parameters` singleton in its constructor from a `repast::Properties` instance. Subclasses of `chi_sim::AbstractModel` will pass a `repast::Properties` instance to their `chi_sim::AbstractModel` superclass, after which the `Parameters` singleton will be available. The key / value pairs specified in the properties file will become named parameters in the `Parameters` singleton.

For example, given the following properties file `model_properties.txt`:

```
model.input.file = ../data/input_file.csv
model.probability = 0.23
```

a model constructor could do the following:

```c++
MyModel::MyModel(repast::Properties& props, MPI_Datatype mpi_person_type) :
        chi_sim::AbstractModel<MyPerson, MyPlace, MyPersonDataType>(props, mpi_person_type)
{

  string input_file = Parameters::instance()->getStringParameter("model.input.file");
  double probability = Parameters::instance()->getDoubleParameter("model.probability");

}
```

As a singleton, the `Parameters` instance is available globally.

#### Calendar ####

The `chi_sim::Calendar` struct is used to represent the current simulated time in minute of the day, hour of the day, day of the week, day of the year, and year. `Calendar` has the following fields:

* `minute_of_day` - begins  at 0
* `hour_of_day` - begins at 0
* `day_of_week` - begin at 0 (Sunday) and ends with 6 (the following Saturday)
* `day_of_year` - begins at 1
* `year` - begins at 1

These fields are public and so client code can implement functions to increment them appropriately, e.g. imcrenemt `minute_of_the_day` by and the the others as appropriate.

`Calendar::increment()` increments the `hour_of_the_day` by 1 and the `minute_of_the_day` by 60. It will also rollover the `day_of_week`, `day_of_year`, and `year` appropriately.

`Calendar::isWeekDay` returns true if the `day_of_week` is not Saturday (6) or Sunday (0).

`Calendar` also has `tick` field that can be used to track the current tick. However, this needs to be set manually and is not modified by `Calendar::increment()`.

As we saw above `Calendar` is used in the required Person's method:

`virtual void selectNextPlace(Calendar& cal, NextPlace<PlaceType>& next_place)`

Consequently, your model code will typically have a `Calendar` field and increment it in some way in a *step* type method, and then pass the instance to `AbstractModel::movePersons`. For example,

```c++
using AbsModelT = chi_sim::AbstractModel<CRXPerson, CRXPlace, CRXPersonDataType>;

class CRXModel: public AbsModelT {

private:
  chi_sim::Calendar cal;
  ...
}


void CRXModel::step() {
    cal.increment();
    movePersons(cal);
    ...
}
```

#### PlaceId ####

`chi_sim::PlaceId` is a struct used to track a place's unique id and rank.

```c++
struct PlaceId {

	int place_id;
	int rank;

	bool doesNotExist() const {
		return place_id == -1;
	}

	bool exists() const {
		return place_id != -1;
	}
};
```

### Implementing main ###

As a RepastHPC-based toolkit, a chiSIM model's `int main(int argc, char **argv)` has some specific requirements. The following can be used as a template.

```c++
#include "repast_hpc/RepastProcess.h"
#include "repast_hpc/initialize_random.h"
#include "repast_hpc/io.h"

using namespace repast;

void usage() {
	std::cerr << "usage: X  model_properties_file" << std::endl;
	std::cerr << "\tmodel_properties_file: the path to the model properties file" << std::endl;
}

void runModel(std::string propsFile, int argc, char** argv) {
	boost::mpi::communicator comm;
  // comment out if timing data to cout is not
  // needed
	if (comm.rank() == 0) {
		std::string time;
		repast::timestamp(time);
		std::cout << "Start Time: " << time << std::endl;
	}

	Properties props(propsFile, argc, argv);
  // initialize the random seed from properties file
	initializeRandom(props);

  // TODO: edit as appropriate
  // call my model's constructor
  MPI_Datatype data_type;
	make_mpi_type(&data_type);
	MyModel model(props, data_type);

  // comment out if timing data to cout is not
  // needed
	if (comm.rank() == 0) {
	  std::string time;
	  repast::timestamp(time);
	  std::cout << "Schedule Start Time: " << time << std::endl;
	}

	RepastProcess::instance()->getScheduleRunner().run();

  // comment out if timing data to cout is not
  // needed
	if (comm.rank() == 0) {
		std::string time;
		repast::timestamp(time);
		std::cout << "End Time: " << time << std::endl;
	}
}

int main(int argc, char **argv) {
	boost::mpi::environment env(argc, argv);
	std::string props;

	boost::mpi::communicator world;

	// if there aren't enough arguments, warn the user and exit
	if (argc < 2) {
		usage();
		return -1;
	}

	// model props file
	props = argv[1];
	if (props.size() > 0) {
		try {
			RepastProcess::init("", &world);
			runModel(props, argc, argv);
			RepastProcess::instance()->done();

		} catch (std::exception& ex) {
			std::cerr << "Error while running the model: " << ex.what() << std::endl;
			throw ex;
		}
	} else {
		if (world.rank() == 0)
			usage();
	}


	return 0;
}
```
