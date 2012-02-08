on_event("MyEvent", function (evt)
    trace("I saw event: " .. evt)
end)

trace()

trace("A test!")

emit("ALuaEvent")
