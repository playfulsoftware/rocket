trace("Registering Lua event handlers...")

on_event("EngineInit", function(evt)
    trace("PSEngine starting up...")
end)

on_event("EngineShutdown", function(evt)
    trace("PSEngine shutting down...")
end)


