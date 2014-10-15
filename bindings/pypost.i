%extend ParameterInstance
{
   double getDouble()
   {
      return *(double*)self->get();
   }
   void setDouble(double val)
   {
      self->set((void*)&val);
   }
   bool getBool()
   {
      return *(bool*)self->get();
   }
   void setBool(bool val)
   {
      self->set((void*)&val);
   }
   %pythoncode %{
      def setValue(self,val):
          if self.type == 0:
              return self.setBool(val)
          elif self.type == 1:
              return self.setDouble(val)
          elif self.type == 2:
              return self.setColor(val)
          elif self.type == 3:
              return self.setPosition(val)
          elif self.type == 4:
              return self.setString(val)
      def getValue(self):
          if self.type == 0:
              return self.getBool()
          elif self.type == 1:
              return self.getDouble()
          elif self.type == 2:
              return self.getColor()
          elif self.type == 3:
              return self.getPosition()
          elif self.type == 4:
              return self.getString()
      def getPosition(self):
          return [0.0,0.0]
      def setPosition(self,val):
          pass
   %}
}
