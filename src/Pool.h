//Pool.h
#ifndef POOL_H_
#define POOL_H_

#include <array>
#include <algorithm>
#include <memory>

// The OBJ template argument supplied by the user
// must have a publicly defined default ctor and a publicly defined
// OBJ::reset() member function that cleans the object innards so that it
// can be reused again (if needed).
template<typename OBJ, int32_t NUM_OBJS>
class Pool {

public:

  //RAII: Fill up the _available array with newly allocated objects.
  //Since all objects are available for the user to "acquire"
  //on initialization, fill up the _inUse array will nullptrs.
  Pool(){

    //Fill the _available array with default-constructed objects
    std::for_each(std::begin(_available), std::end(_available),
        [](OBJ*& mem){mem = new OBJ{};});

    //Since no objects are currently in use, fill the _inUse array with nullptrs
    std::for_each(std::begin(_inUse), std::end(_inUse),
        [](OBJ*& mem){mem = nullptr;});

  }

  //Allows a user to acquire an object to manipulate as she wishes
  OBJ* acquire() {

    //Try to find an available object
    auto iter = std::find_if(std::begin(_available), std::end(_available),
        [](OBJ* mem){return (mem != nullptr);});

    //Is the pool exhausted?
    if(iter == std::end(_available)) {

      return nullptr; //Replace this with a "throw" if you need to.

    }
    else {

      //Whoo hoo! An object is available for usage by the user
      OBJ* mem{*iter};

      //Mark it as in-use in the _available array
      *iter = nullptr;

      //Find a spot for it in the _inUse list
      //iter = std::find_if(std::begin(_inUse), std::end(_inUse),
          //[](OBJ* mem){return (mem == nullptr);});
      iter = std::find(std::begin(_inUse), std::end(_inUse), nullptr);

      //Insert it into the _inUse list. We don't
      //have to check for iter == std::end(_inUse)
      //because we are fully in control!
      *iter = mem;

      //Finally, return the object to the user
      return mem;

    }

  }

  //Returns an object to the pool
  void release(OBJ* obj) {

    //Ensure that the object is indeed in-use. If
    //the user gives us an address that we don't own,
    //we'll have a leak and bad things can happen when we
    //delete the memory during Pool destruction.
    auto iter = std::find(std::begin(_inUse), std::end(_inUse), obj);

    //Simply return control back to the user
    //if we didn't find the object in our
    //in-use list
    if(iter == std::end(_inUse)) {

        return;

    }
    else {

      //Clear out the innards of the object being returned
      //so that the next user doesn't have to "remember" to do it.
      obj->reset();

      //temporarily hold onto the object while we mark it
      //as available in the _inUse array
       OBJ* mem{*iter};

       //Mark it!
       *iter = nullptr;

       //Find a spot for it in the _available list
       //iter = std::find_if(std::begin(_available), std::end(_available),
           //[](OBJ* mem){return(mem == nullptr);});
       iter = std::find(std::begin(_available), std::end(_available), nullptr);

       //Insert the object's address into the _available array
       *iter = mem;

    }

  }

  ~Pool() {

    //Cleanup after ourself: RAII
    std::for_each(std::begin(_available), std::end(_available),
        std::default_delete<OBJ>());

    std::for_each(std::begin(_inUse), std::end(_inUse),
        std::default_delete<OBJ>());

  }

private:

  //Keeps track of all objects available to users
  std::array<OBJ*, NUM_OBJS> _available;

  //Keeps track of all objects in-use by users
  std::array<OBJ*, NUM_OBJS> _inUse;

};

#endif /* POOL_H_ */
