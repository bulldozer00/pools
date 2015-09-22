//Pool.h
#ifndef POOL_H_
#define POOL_H_

#include <algorithm>
#include <set>
#include <memory>

// The OBJ template argument supplied by the user
// must have a publicly defined default ctor and a publicly defined
// OBJ::reset() member function that cleans the object innards so that it
// can be reused again (if needed).
template<typename OBJ, int32_t NUM_OBJS>
class Pool {

public:

  //RAII: Fill up the _available set with newly allocated objects.
  //Since all objects are available for the user to "acquire"
  //on initialization, fill up the _inUse set will nullptrs.
  Pool(){

    for(int32_t i=0; i<NUM_OBJS; ++i) {

      _available.insert(new OBJ{});
      _inUse.insert(nullptr);

    }

  }

  //Allows a user to acquire an object to manipulate as she wishes
  OBJ* acquire() {

    //Try to find an available object
    auto iter = std::find_if(std::begin(_available), std::end(_available),
        [](OBJ* mem){return (mem not_eq nullptr);});

    //Is the pool exhausted?
    if(iter == std::end(_available)) {

      return nullptr; //Replace this with a "throw" if you need to.

    }
    else {

      //Whoo hoo! An object is available for usage by the user
      OBJ* mem{*iter};

      //Mark it as in-use in the _available set
      _available.erase(iter);
      _available.insert(nullptr);

      //Find a spot for it in the _inUse set
      iter = std::find(std::begin(_inUse), std::end(_inUse), nullptr);

      //Insert it into the _inUse set. We don't
      //have to check for iter == std::end(_inUse)
      //because we are fully in control!
      _inUse.erase(iter);
      _inUse.insert(mem);

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
    //in-use set
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
       _inUse.erase(iter);
       _inUse.insert(nullptr);

       //Find a spot for it in the _available list
       iter = std::find(std::begin(_available), std::end(_available), nullptr);

       //Insert the object's address into the _available array
       _available.erase(iter);
       _available.insert(mem);
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
  std::multiset<OBJ*> _available;

  //Keeps track of all objects in-use by users
  std::multiset<OBJ*> _inUse;

};

#endif /* POOL_H_ */
