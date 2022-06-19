# JUCE Event System Prototype

This is a prototype of an event system where the event channels are type based. So for each EventSystem instance any Event type will be sent / received on the same channel.

You can use any type you like as the event payload and a handler will be automatically removed if you don't keep it alive.

## Usage examples

```cpp
    struct SimpleEvent
    {
        juce::String data {};
    };

    auto mainHandler = eventSystem.add<SimpleEvent>([](SimpleEvent event)
    {
        DBG("Main Handler : " + event.data);
    });

    eventSystem.send(SimpleEvent{"Event One"});

    struct ComplexEvent
    {
        int id = 0;
        juce::String name {};
    };

    auto complexHandler = eventSystem.add<ComplexEvent>(
        [](auto event)
        {
            DBG("Complex Event " + juce::String(event.id) + " -> " + event.name);
        });

    eventSystem.send(ComplexEvent{23, "Normalised"});
```