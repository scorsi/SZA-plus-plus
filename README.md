## SZA (Simple Zia Api)

Hi and welcome to this minimalist API. The point of this API is to restrict in no way your conception or design choices.

By giving a simple Module interface, we believe that every group will be able to build its own Zia and not just the Zia that our API require you to create.

### Dependencies

 - C++ 17

### How to build

Simply clone this repository and include the .h files in the **api** folder.

### What does SZA do :

It introduces four header files, each commented to show explicit usage :
 - conf.h
```markdown
Conf is a simple map used to configure your module, it is inspired by the JSON format.
By making ConfValue a std::variant we allow user to choose themselves how they want to configure their module.
```
 - net.h
```markdown
Contains :
 - The NetInfo struct and its components, used to provide infos on the client 
 - The Net Interface for multithreaded network abstraction
```
 - http.h
```markdown
Contains :
 - The HttpMessage struct, it contains the version, the headers and the packets
 - The HttpRequest struct, it inherits from HttpMessage and add a Method enum class and an Uri as a String
 - The HttpResponse struct, it inherits from HttpMessage and add a Status enum class

 - The **HttpDuplex** struct, it contains :
	1. A NetInfo struct,
	2. A packets vector for req and another for resp
	3. An HttpRequest struct and an HttpResponse struct.
 This struct is the _core_ of our API and should be the one used during the entire processing of the request.
```
 - module.h
```markdown
 Module is the interface to use for your module, it contains two method :
 - config, it takes a const Conf& and is used to configure your module depending on the state of your Conf
 - exec, it takes an HttpDuplex and should be the method that "applies" your module to the current HttpDuplex
```

### How to use it :

After loading your modules, just simply use something like :

```markdown
HttpDuplex http;
moduleReceive.exec(http);
moduleTreatRequest.exec(http);
moduleSend.exec(http);
```

Absolutely no need for any complicated conception when you can just use SZA.
