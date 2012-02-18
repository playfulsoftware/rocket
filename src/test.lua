Object = {}

function Object:dispatch(evt)
    if self["on"..evt] ~= nil then
        self["on"..evt](self)
    end
end

function Object:new(prototype)
    prototype = prototype or {}
    setmetatable(o, self)
    self.__index = self
    return prototype
end

trace("Registering Lua event handlers...")

on_event("EngineInit", function()
    trace("PSEngine starting up...")
end)

on_event("EngineShutdown", function()
    trace("PSEngine shutting down...")
end)


