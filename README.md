# shellso

A interactive shell for the Operational Systems (OS) at UFSJ.

```mermaid
graph
   DF((DF))--0/0-->E((E))
   E((E))--0/0-->C((C))
   A((A))--1/0-->B((B))
   E((E))--1/1-->DF((DF))
   A((A))--0/0-->C((C))
   C((C))--0/0-->C((C))
   <!-- DF((DF))--1/0-->A((A)) -->
   B((B))--0/0-->C((C))
   B((B))--1/0-->A((A))
   C((C))--1/0-->DF((DF))
```