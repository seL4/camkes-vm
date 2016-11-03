Demonstration of Cross VM Connections
=====================================

This app contains a vmm component that boots a guest linux in a vm. An
additional component contains logic for reversing a string in a dataport,
whose actions are coordinated by events. This component is connected to linux
userland, and an application in the linux filesystem uses this component to
reverse strings, demonstrating cross vm connections.
