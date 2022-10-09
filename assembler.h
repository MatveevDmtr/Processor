
#define allocate_array(type, num_elems)                             \
({                                                                  \
    type* location__ = (type*) calloc (num_elems, sizeof (type));   \
    log ("Type_calloc: %s\n", #type);                               \
    log ("Sizeof type: %d\n", sizeof (type*));                      \
    Assert(location__ == NULL);                                     \
    log ("Calloc returned: %p\n\n", location__);                    \
    location__;                                                     \
})


#define safe_def_int(func, invalid_value)     \
({                                            \
    int variable__ = func;                    \
    Assert(variable__ == invalid_value);      \
    variable__;                               \
})
