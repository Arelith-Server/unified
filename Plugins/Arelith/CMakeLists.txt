find_package(OpenSSL)

if (${OPENSSL_FOUND})
     add_plugin(Arelith
        "Arelith.cpp"
        "Source/ArelithEvents.cpp"
        "Source/AttackOfOpportunity.cpp"
        "Source/CanUse.cpp"
        "Source/DoorsNoBudge.cpp"
        "Source/GetItemAppearance.cpp"
        "Source/MaxStorePricePerStack.cpp"
        "Source/WeaponPower.cpp"
        "Source/ItemPosition.cpp")
    target_link_libraries(Arelith ${OPENSSL_LIBRARIES})
    target_include_directories(Arelith PUBLIC ${OPENSSL_INCLUDE_DIR})
endif()
